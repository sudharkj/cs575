#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

// setting the number of threads:
#ifndef NUMT
#define NUMT 1
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES 10
#endif

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

// interesting parameters

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

//Units of grain growth are inches.
//Units of temperature are degrees Fahrenheit (°F).
//Units of precipitation are inches.

unsigned int seed = 0;

// state of the system

int	NowYear;		// 2020 - 2025
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	    NowNumDeer;		// number of deer in the current population
int	    NowNumAgent;		// number of deer in the current population

float
SQR( float x )
{
    return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX

    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
    float low = (float)ilow;
    float high = (float)ihigh + 0.9999f;

    return (int)(  Ranf(seedp, low,high) );
}

float x = Ranf( &seed, -1.f, 1.f );

void SetState() {
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
    if( NowPrecip < 0. )
        NowPrecip = 0.;
}

void GrainDeer() {
    while( NowYear < 2026 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        int CurrentNumDeer = NowNumDeer;
        if (NowMonth == 11 && CurrentNumDeer > NowNumAgent) {
            CurrentNumDeer -= NowNumAgent;
        }
        if ((float) CurrentNumDeer < NowHeight) {
            CurrentNumDeer += 1;
        }
        if ((float) CurrentNumDeer > NowHeight) {
            CurrentNumDeer -= 1;
        }
        if (CurrentNumDeer < 0) {
            CurrentNumDeer = 0;
        }

        // DoneComputing barrier:
//        printf("[GrainDeer] Done Computing: (%d)\n", CurrentNumDeer);
//        printf("[GrainDeer] Waiting for DoneComputing barrier\n");
#pragma omp barrier

        NowNumDeer = CurrentNumDeer;
        // DoneAssigning barrier:
//        printf("[GrainDeer] Done Assigning: (%d)\n", NowNumDeer);
//        printf("[GrainDeer] Waiting for DoneAssigning barrier\n");
#pragma omp barrier

        // DonePrinting barrier:
//        printf("[GrainDeer] Nothing to Print\n");
//        printf("[GrainDeer] Waiting for DonePrinting barrier\n");
#pragma omp barrier

    }
}

void GrainGrowth() {
    while( NowYear < 2026 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:

        float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
        float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

        float CurrentHeight = NowHeight;
        if (CurrentHeight > NowNumAgent) {
            CurrentHeight -= NowNumAgent;
        }
        CurrentHeight += (float)NowNumAgent * tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        CurrentHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
        if (CurrentHeight < 0.) {
            CurrentHeight = 0.;
        }

        // DoneComputing barrier:
//        printf("[GrainGrowth] Done Computing: (%.2lf)\n", CurrentHeight);
//        printf("[GrainGrowth] Waiting for DoneComputing barrier\n");
#pragma omp barrier

        NowHeight = CurrentHeight;
        // DoneAssigning barrier:
//        printf("[GrainGrowth] Done Assigning: (%.2lf)\n", NowHeight);
//        printf("[GrainGrowth] Waiting for DoneAssigning barrier\n");
#pragma omp barrier

        // DonePrinting barrier:
//        printf("[GrainGrowth] Nothing to Print\n");
//        printf("[GrainGrowth] Waiting for DonePrinting barrier\n");
#pragma omp barrier
    }
}

void MyAgent() {
    while( NowYear < 2026 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:

        float CurrentNumAgent = NowNumAgent;
        if (NowMonth == 11) {
            if (CurrentNumAgent < NowNumDeer) {
                CurrentNumAgent += 1;
            }
            if (CurrentNumAgent < NowHeight) {
                CurrentNumAgent += 1;
            }
            if (CurrentNumAgent > NowNumDeer) {
                CurrentNumAgent -= 1;
            }
            if (CurrentNumAgent > NowHeight) {
                CurrentNumAgent -= 1;
            }
            if (CurrentNumAgent < 2) {
                CurrentNumAgent = 2;
            }
        }

        // DoneComputing barrier:
//        printf("[MyAgent] Done Computing: (%d)\n", CurrentNumAgent);
//        printf("[MyAgent] Waiting for DoneComputing barrier\n");
#pragma omp barrier

        NowNumAgent = CurrentNumAgent;
        // DoneAssigning barrier:
//        printf("[MyAgent] Done Assigning: (%d)\n", NowNumAgent);
//        printf("[MyAgent] Waiting for DoneAssigning barrier\n");
#pragma omp barrier

        // DonePrinting barrier:
//        printf("[MyAgent] Nothing to Print\n");
//        printf("[MyAgent] Waiting for DonePrinting barrier\n");
#pragma omp barrier
    }
}

void Watcher() {
    while( NowYear < 2026 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:

        // DoneComputing barrier:
//        printf("[Watcher] Nothing to Compute\n");
//        printf("[Watcher] Waiting for DoneComputing barrier\n");
#pragma omp barrier

        // DoneAssigning barrier:
//        printf("[Watcher] Nothing to Assign\n");
//        printf("[Watcher] Waiting for DoneAssigning barrier\n");
#pragma omp barrier
        printf("%d,%d,%.2lf,%.2lf,%.2lf,%d,%d\n",
                NowYear, NowMonth, NowPrecip, NowTemp, NowHeight, NowNumDeer, NowNumAgent);

        NowMonth += 1;
        if (NowMonth > 11) {
            NowYear += 1;
            NowMonth = 0;
        }
        SetState();
        // DonePrinting barrier:
//        printf("[Watcher] Done Printing: (%.2lf, %.2lf)\n", NowTemp, NowPrecip);
//        printf("[Watcher] Waiting for DonePrinting barrier\n");
#pragma omp barrier

    }
}

int main() {
    #ifndef _OPENMP
    fprintf(stderr, "No OpenMP support!\n");
    return 1;
    #endif

    // starting date and time:
    NowMonth =    0;
    NowYear  = 2020;

// starting state (feel free to change this if you want):
    NowNumDeer = 6;
    NowHeight =  12.;
    NowNumAgent = 2;
    SetState();

    unsigned int seed = 0;

    printf("Year,Month,Precipitation,Temperature,Height,Deers,Humans\n");
//    omp_set_num_threads( 3 );	// same as # of sections
    omp_set_num_threads( 4 );	// same as # of sections
#pragma omp parallel sections
    {
#pragma omp section
        {
            GrainDeer( );
        }

#pragma omp section
        {
            GrainGrowth( );
        }

#pragma omp section
        {
            Watcher( );
        }

#pragma omp section
        {
            MyAgent( );	// your own
        }
    }       // implied barrier -- all functions must return in order
    // to allow any of them to get past here


    return 0;
}
