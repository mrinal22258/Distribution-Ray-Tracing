TO COMPILE MY CODE :

STEP 1 :
cd code_Project_2_Group_20

STEP 2 :
cmake CMakeLists.txt

STEP 3 :
make

STEP 4 :
./ray_tracer

The current code works for DoF
we can change the focus based on line 71 in main.cpp (2->3->4)


For using the Motion Blur and Soft Shadows
Comment as mentioned in DoF

Then Use:
To play around with values do changes in line (157-159) in raytracer.cpp
Futhermore to change between 1ray/px to multi rays
Do the changes as mentioned in comments in:
raytracer.hpp -> line(12-16)
raytracer.cpp -> line(34-78)
main.cpp -> line(115-126)

Lastly, again to play around with values do changes in line (157-159) in raytracer.cpp
