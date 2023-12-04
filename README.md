# hyperbolic-isometries
Danielle Simon

# Description
This project computes and displays orientation preserving hyperbolic isometries in the upper half plane model.

# Quickstart
To run the program enter ./isom -a [real number] -b [real number] -c [real number] -d [real number]

# Program commands
Press s to enter add points mode \
Press e to exit add points mode \
Press i to see the isometry \
Press r to revert to the initial input points \
Press c to revert to clear the points \
Press g to generate 20 random points \
Press h to see the help menu

# Notes for choosing a,b,c,d
1. ad-bc must equal 1
2. if (a+d)^2 > 4, a loxodromic isometry will be displayed
3. if (a+d)^2 = 4, a parabolic isometry will be displayed
4. if (a+d)^2 < 4, an elliptic isometry will be displayed

# Sample commands

**Loxodromic:**

f(2z) in disguise w/ a = sqrt(2) and d = 1/a (this fixes and sends points to infinity): \
./isom -a 1.414214 -b 0 -c 0 -d 0.707107

These make points approach a number on the real line (note: points get infititely close to a y-coordinate of 0): \
./isom -a 5 -b 7 -c 2 -d 3 \
./isom -a 2 -b 1 -c 1 -d 1 \
./isom -a 5 -b -1 -c 1 -d 0

**Elliptic**

Rotation of PI/4 degrees: \
./isom -a 0.923880 -b 0.382683 -c -0.382683 -d 0.923880

Other rotations with large radii: \
./isom -a -.25 -b -5 -c .25 -d 1 \
./isom -a -1 -b -2 -c 1 -d 1 \
./isom -a -1 -b -1 -c 1 -d 0

**Parabolic**

Right shift: \
./isom -a 1 -b 4 -c .0 -d 1

Other parabolic isometries: \
./isom -a 2 -b -1 -c 1 -d 0
