#include <iostream>
#include <iomanip>
#include <omp.h>
#include <string.h>

using namespace std;

#include "CL/cl.h"
#include "CL/cl_helper.h"

// opencl vendor ids:
#define ID_AMD 0x1002
#define ID_INTEL 0x8086
#define ID_NVIDIA 0x10de

cl_platform_id platform;
cl_device_id device;

void printOpenCLInfo() {
    cl_int status; // returned status from opencl calls
    cerr << "PrintInfo:" << endl;

    // find out how many platforms are attached here:
    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    printCLError(status, "clGetPlatformIDs failed (1)");
    cerr << "Number of Platforms = " << numPlatforms << endl;

    // get platform ids:
    cl_platform_id *platforms = new cl_platform_id[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    printCLError(status, "clGetPlatformIDs failed (2)");

    for (int p = 0; p < (int) numPlatforms; p++) {
        cerr << "Platform #" << p << endl;
        // declare common variables:
        size_t size;
        char *str;

        // print platform name:
        clGetPlatformInfo(platforms[p], CL_PLATFORM_NAME, 0, NULL, &size);
        str = new char[size];
        clGetPlatformInfo(platforms[p], CL_PLATFORM_NAME, size, str, NULL);
        cerr << "\tName    = " << str << endl;
        delete[] str;

        // print platform vendor:
        clGetPlatformInfo(platforms[p], CL_PLATFORM_VENDOR, 0, NULL, &size);
        str = new char[size];
        clGetPlatformInfo(platforms[p], CL_PLATFORM_VENDOR, size, str, NULL);
        cerr << "\tVendor  = " << str << endl;
        delete[] str;

        // print platform version:
        clGetPlatformInfo(platforms[p], CL_PLATFORM_VERSION, 0, NULL, &size);
        str = new char[size];
        clGetPlatformInfo(platforms[p], CL_PLATFORM_VERSION, size, str, NULL);
        cerr << "\tVersion = " << str << endl;
        delete[] str;

        // print platform profile:
        clGetPlatformInfo(platforms[p], CL_PLATFORM_PROFILE, 0, NULL, &size);
        str = new char[size];
        clGetPlatformInfo(platforms[p], CL_PLATFORM_PROFILE, size, str, NULL);
        cerr << "\tProfile = " << str << endl;
        delete[] str;

        // find out how many devices are attached to the platform:
        cl_uint numDevices;
        status = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
        printCLError(status, "clGetDeviceIDs failed (1)");
        cerr << "\tNumber of Devices = " << numDevices << endl;

        // get device ids:
        cl_device_id *devices = new cl_device_id[numDevices];
        status = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
        printCLError(status, "clGetDeviceIDs failed (2)");

        // print device details:
        for (int d = 0; d < (int) numDevices; d++) {
            cerr << "\tDevice #" << d << ":" << endl;
            // declare common device variables:
            size_t size;
            cl_device_type type;
            cl_uint ui;
            size_t sizes[3] = {0, 0, 0};

            // print device type:
            clGetDeviceInfo(devices[d], CL_DEVICE_TYPE, sizeof(type), &type, NULL);
            cerr << "\t\tType = " << showbase << setfill('0') << setw(4) << hex << (unsigned int) type << dec << " = ";
            switch (type) {
                case CL_DEVICE_TYPE_CPU:
                    cerr << "CL_DEVICE_TYPE_CPU" << endl;
                    break;
                case CL_DEVICE_TYPE_GPU:
                    cerr << "CL_DEVICE_TYPE_GPU" << endl;
                    break;
                case CL_DEVICE_TYPE_ACCELERATOR:
                    cerr << "CL_DEVICE_TYPE_ACCELERATOR" << endl;
                    break;
                default:
                    cerr << "Other..." << endl;
                    break;
            }

            // print device vendor:
            clGetDeviceInfo(devices[d], CL_DEVICE_VENDOR_ID, sizeof(ui), &ui, NULL);
            cerr << "\t\tDevice Vendor ID = " << showbase << setfill('0') << setw(4) << hex << ui << dec;
            switch (ui) {
                case ID_AMD:
                    cerr << " (AMD)" << endl;
                    break;
                case ID_INTEL:
                    cerr << " (Intel)" << endl;
                    break;
                case ID_NVIDIA:
                    cerr << " (NVIDIA)" << endl;
                    break;
                default:
                    cerr << " (?)" << endl;
            }

            // print device maximum compute units:
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(ui), &ui, NULL);
            cerr << "\t\tDevice Maximum Compute Units = " << ui << endl;

            // print device maximum work item dimensions:
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(ui), &ui, NULL);
            cerr << "\t\tDevice Maximum Work Item Dimensions = " << ui << endl;

            // print device maximum work item sizes:
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(sizes), sizes, NULL);
            cerr << "\t\tDevice Maximum Work Item Sizes = " << (int) sizes[0] << " x "
                 << (int) sizes[1] << " x " << (int) sizes[2] << endl;

            // print device maximum work group size:
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size), &size, NULL);
            cerr << "\t\tDevice Maximum Work Group Size = " << (int) size << endl;

            // print device maximum clock frequency:
            clGetDeviceInfo(devices[d], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(ui), &ui, NULL);
            cerr << "\t\tDevice Maximum Clock Frequency = " << ui << " MHz" << endl;

            // print device extensions:
            size_t extensionSize;
            clGetDeviceInfo(devices[d], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
            char *extensions = new char[extensionSize];
            clGetDeviceInfo(devices[d], CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);
            cerr << endl << "Device #" << d << "'s Extensions:" << endl;
            for (int e = 0; e < (int) strlen(extensions); e++) {
                if (extensions[e] == ' ')
                    extensions[e] = '\n';
            }
            cerr << extensions << endl;
            delete[] extensions;
        }
        delete[] devices;
    }
    delete[] platforms;
    cerr << endl;
}


void selectOpenCLDevice() {
    // select which opencl device to use:
    // priority order:
    //	1. a gpu
    //	2. an nvidia or amd gpu

    int iplatform = -1;
    int idevice = -1;
    cl_device_type deviceType;
    cl_uint deviceVendor;
    cl_int status;  // returned status from opencl calls

    cerr << "Selecting the OpenCL Platform and Device:" << endl;

    // find out how many platforms are attached here:
    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    printCLError(status, "clGetPlatformIDs failed (1)");
    cerr << "Number of Platforms = " << numPlatforms << endl;

    // get platform ids:
    cl_platform_id *platforms = new cl_platform_id[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    printCLError(status, "clGetPlatformIDs failed (2)");

    for (int p = 0; p < (int) numPlatforms; p++) {
        // find out how many devices are attached to the platform:
        cl_uint numDevices;
        status = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
        printCLError(status, "clGetDeviceIDs failed (1)");
        cerr << "\tNumber of Devices = " << numDevices << endl;

        // get device ids:
        cl_device_id *devices = new cl_device_id[numDevices];
        status = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
        printCLError(status, "clGetDeviceIDs failed (2)");

        for (int d = 0; d < (int) numDevices; d++) {
            cl_device_type type;
            cl_uint vendor;
            size_t sizes[3] = {0, 0, 0};

            // get device type
            clGetDeviceInfo(devices[d], CL_DEVICE_TYPE, sizeof(type), &type, NULL);
            // get device vendor
            clGetDeviceInfo(devices[d], CL_DEVICE_VENDOR_ID, sizeof(vendor), &vendor, NULL);

            // select:
            if (iplatform < 0) {
                // not yet holding anything -- we'll accept anything
                iplatform = p;
                idevice = d;
                platform = platforms[iplatform];
                device = devices[idevice];
                deviceType = type;
                deviceVendor = vendor;
            } else {
                // holding something already -- can we do better?
                if (deviceType == CL_DEVICE_TYPE_CPU) {
                    // holding a cpu already -- switch to a gpu if possible
                    if (type == CL_DEVICE_TYPE_GPU) {
                        // found a gpu - switch to the gpu we just found
                        iplatform = p;
                        idevice = d;
                        platform = platforms[iplatform];
                        device = devices[idevice];
                        deviceType = type;
                        deviceVendor = vendor;
                    }
                } else {
                    // holding a gpu -- is a better gpu available?
                    if (deviceVendor == ID_INTEL) {
                        // currently holding an intel gpu - we are assuming we just found a bigger, badder nvidia or amd gpu
                        iplatform = p;
                        idevice = d;
                        platform = platforms[iplatform];
                        device = devices[idevice];
                        deviceType = type;
                        deviceVendor = vendor;
                    }
                }
            }
        }
    }

    if (iplatform < 0) cerr << "Found no OpenCL devices!" << endl;
    else cerr << "I have selected Platform #"<< iplatform << ", Device #" << idevice << endl;
}

int main() {
    printOpenCLInfo();
    selectOpenCLDevice();
    return 0;
}
