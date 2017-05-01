#!/bin/bash
##

projectname=$1

if [ -z "$projectname" ]; then
    echo "No project name specified. Do nothing. Exit."
	exit 
fi

cp -r project_template $projectname

rm $projectname/readme.template.txt

mv $projectname/include/TEMPLATE $projectname/include/$projectname

# echo "Don't forget to add the following command to the root CMakeLists.txt"
echo "add_subdirectory($projectname)" >> CMakeLists.txt

find $projectname/ -type f -name "*.txt" -print0 | xargs -0 sed -i "s/TEMPLATE/$projectname/g"
find $projectname/ -type f -name "*.cpp" -print0 | xargs -0 sed -i "s/TEMPLATE/$projectname/g"
find $projectname/ -type f -name "*.h"   -print0 | xargs -0 sed -i "s/TEMPLATE/$projectname/g"
