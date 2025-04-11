#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <optional>
#include <vector>

#include "vec.h"
#include "ray.h"

using namespace cg;

int main() {

  /*
  ============================================================================
   Task 0
  ============================================================================
   Implement the tasks given in the comments below. Read carefully! The tasks
   are meant to be solved in the order they are presented in.
   Look up C++ syntax and how to use it if needed.

   The skeleton code of the raytracer uses rather modern C++ syntax that is
   sometimes not obvious and hard to understand. If you encounter a piece
   of code you don't know how to interpret even after researching yourself,
   ask your instructor for help.
  */

  /* This is an example for creating a vector that has a one in each component
        vec3 ones{ 1.0, 1.0, 1.0 };
     an alternative way of writing is
        vec3 ones(1.0, 1.0, 1.0);
     or also
        vec3 ones = vec3(1.0, 1.0, 1.0);
  */

  // This is an example vector
  vec3 v_ex{ 1.0, 2.0, 3.0 };


  // ---------------------------------------------------------------------------


  // Create a unit vector for the x, y and z direction.
  vec3 v_x{ 1, 0, 0 };
  vec3 v_y{ 0, 1, 0 };
  vec3 v_z{ 0, 0, 1 };

  

  // ---------------------------------------------------------------------------


  // Compute the dot product between the example vector v_ex and each of the unit vectors.
  // Output the results on the console with std::cout. 
  // Use the function "dot" (see vec.h) that is already provided.
  
  double ex_dot_x{ dot(v_ex, v_x) };
  double ex_dot_y{ dot(v_ex, v_y) };
  double ex_dot_z{ dot(v_ex, v_z) };
  
  std::cout << "Result of dot products: " << ex_dot_x << " " << ex_dot_y << " " << ex_dot_z << std::endl;
  

  // ---------------------------------------------------------------------------


  // Reconstruct the example vector v_ex as a linear combination of the unit vectors. 
  // You can use the results from the previous tasks.
  // Check if your reconstruction and the original vector are the same.
  
  vec3 v_ex_reconstruct{ v_x + 2 * v_y + 3 * v_z};

  bool isSame = v_ex_reconstruct(0) == v_ex(0) && v_ex_reconstruct(1) == v_ex(1) &&
    v_ex_reconstruct(2) == v_ex(2);
  if(isSame){
    std::cout << "The reconstructed vector is the same as the original vector." << std::endl;
  }
  else{
    std::cout << "The reconstructed vector and the original vector are different." << std::endl;
  }


  // ---------------------------------------------------------------------------


  // Construct a vector that is orthogonal to the example vector v_ex by using the cross product.
  // Check that property by using the dot product.
  // Use the functions "dot" and "cross" (see vec.h) that are already provided.

  vec3 v_orth{ cross(v_ex, v_x) };

  bool isOrthogonal = dot(v_orth, v_ex) == 0;
  if(isOrthogonal){
    std::cout << "The vector is orthogonal to v_ex." << std::endl;
  }
  else{
    std::cout << "The vector is not orthogonal to v_ex." << std::endl;
  }


  // ---------------------------------------------------------------------------


  // v_ex_2 is another example vector.
  // Change the _third_ component of this vector to 4.
  // Output the length of the vector to the console by using std::cout 
  // and the member function "length" from the vector class (see vec.h).
  // Normalize the vector by using the function "normalize" (see vec.h) that is already provided.
  // Output the length of the vector again.

  vec3 v_ex_2 {2.0, 3.0, 42.0};
  v_ex_2(2) = 4;

  std::cout << "Length of v_ex_2 before normalization: " << v_ex_2.length() << std::endl;

  std::cout << "Length of v_ex_2 after normalization: " << normalize(v_ex_2).length() << std::endl;

  
  // ---------------------------------------------------------------------------


  // Construct a Ray object r that has v_ex as its origin and v_ex_2 as its direction.
  // Evaluate the ray r at the time t by using the ()-operator 
  // and output the result at the console.
  // See ray.h for the definitions of the constructor and the operator.

  double t = 9.25;
  Ray r{ v_ex, v_ex_2 };

  std::cout << "The ray at time t = " << t << " is the position " << r(t) << std::endl;
  

  // ---------------------------------------------------------------------------


  // Determine the angle between the ray direction and the y-axis by using
  // the member function "direction" (see ray.h), "dot" and "normalize" (see vec.h)
  // and "std::acos" where appropriate.
  // Output whether the angle is greater or smaller than 45 degree.

  double angle_radians{ std::acos(dot(v_y, normalize(r.direction()))) };
  double angle_degrees{ angle_radians * 180 / M_PI };

  if (angle_degrees > 45) {
    std::cout << "The angle is greater than 45 degrees. (" << angle_degrees << " / " << angle_radians << ")" << std::endl;
  }
  else {
    std::cout << "The angle is smaller than 45 degrees. (" << angle_degrees << " / " << angle_radians << ")" << std::endl;
  }
  

  // ---------------------------------------------------------------------------


  // Given below is a std::vector (the C++ equivalent to a resizable array/list) that holds
  // objects of the type "vec3".
  // For each of the vec3, output their distance to the point v_ex on the console.
  // Use a range based for loop 
  //    (https://en.cppreference.com/w/cpp/language/range-for)
  //    (https://docs.microsoft.com/en-us/cpp/cpp/range-based-for-statement-cpp?view=msvc-170)
  // to iterate over the collection. You don't have to use references or the auto keyword.

  std::vector<vec3> points { vec3{0.0, 0.0, 0.0}, vec3{10.0, 2.0, -1.0}, vec3{-3, -1, -42} , vec3{2.4, -3.6, 8.43}};

  for (vec3 v : points) {
    std::cout << "The distance of " << v << " to " << v_ex << " is " << distance(v, v_ex) << std::endl;
  }

  

  // ---------------------------------------------------------------------------


  // Given below is are 2 objects of the class std::optional that can hold a value of the type
  // vec3. As the name suggests, an optional CAN hold a value of that type but it can also
  // be empty.
  // For each of the two optionals opt_1 and opt_2 do the following 
  // IF the optional contains a value:
  //  - output the value (vec3) to the console
  //  - output the length of the value (vec3) to the console
  // else
  //  - output that the optional does not contain a value
  // See also 
  //    (https://en.cppreference.com/w/cpp/utility/optional)
  //    (https://www.cppstories.com/2018/05/using-optional/#accessing-the-stored-value)
  // To avoid duplicating the code, you can use the skeleton of the loop provided below.
  // The keyword "auto" replaces the need to explicitly write the type of the loop variable.

  std::optional<vec3> opt_1;
  std::optional<vec3> opt_2{vec3{0.0, 2.0, 1.0}};

  for(auto& opt : {opt_1, opt_2}){
    if (!opt.has_value()) {
      std::cout << "The optional does not contain a value :(" << std::endl;
    }
    else {
      std::cout << "The optional contains the value " << opt.value() << " which has the length " <<
        opt.value().length() << std::endl;
    }
  }
}