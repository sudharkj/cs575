## CS 575 Projects

Contains course projects of CS 575.

#### Projects Structure

* *README.md* contains details specific to the project
* *proj.cpp* contains the OpenMP related CPP code
* *analyze_results.py* contains python scripts to analyze the results
* *Makefile* contains the make commands for CPP code
* *start.sh* is the bash script to the run the project

#### Running any project

```bash
./start.sh
```

#### Submission

All the following commands are from parent folder of the related project.

```bash
# create README file from report
pandoc [FOLDER_NAME]/latex/report.tex -o [FOLDER_NAME]/README2.md
# make any manual changes to the file
mv -f [FOLDER_NAME]/README2.md [FOLDER_NAME]/README.md

# clean all generated files

# reset submission folder
rm -rf submission
mkdir submission

# move report
mv proj0x/latex/[REPORT_NAME].pdf submission/

# zip project and move to submission
zip -r [ZIP_NAME].zip [FOLDER_NAME]
mv [ZIP_NAME].zip submission/
```

#### Troubleshooting

**Unable to run any script**

Run `chmod u+x start.sh` and then run `./start.sh` from the folder.
