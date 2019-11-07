//
// Copyright (c) 2015-2019 CNRS INRIA
// Copyright (c) 2016 Wandercraft, 86 rue de Paris 91400 Orsay, France.
//

#ifndef __pinocchio_python_force_hpp__
#define __pinocchio_python_force_hpp__

#include <eigenpy/memory.hpp>
#include <boost/python/tuple.hpp>

#include "pinocchio/spatial/se3.hpp"
#include "pinocchio/spatial/force.hpp"
#include "pinocchio/bindings/python/utils/copyable.hpp"
#include "pinocchio/bindings/python/utils/printable.hpp"

EIGENPY_DEFINE_STRUCT_ALLOCATOR_SPECIALIZATION(pinocchio::Force)

namespace pinocchio
{
  namespace python
  {
    namespace bp = boost::python;
  
    template<typename T> struct call_isApprox;
  
    template<typename Scalar, int Options>
    struct call_isApprox< ForceTpl<Scalar,Options> >
    {
      typedef ForceTpl<Scalar,Options> Force;
      
      static bool run(const Force & self, const Force & other,
                      const Scalar & prec = Eigen::NumTraits<Scalar>::dummy_precision())
      {
        return self.isApprox(other,prec);
      }
    };

    BOOST_PYTHON_FUNCTION_OVERLOADS(isApproxForce_overload,call_isApprox<Force>::run,2,3)

    template<typename Force>
    struct ForcePythonVisitor
    : public boost::python::def_visitor< ForcePythonVisitor<Force> >
    {
      enum { Options = traits<Motion>::Options };
      
      typedef typename Force::Vector6 Vector6;
      typedef typename Force::Vector3 Vector3;
      typedef typename Force::Scalar Scalar;
      
      template<class PyClass>
      void visit(PyClass& cl) const 
      {
        cl
        .def(bp::init<>("Default constructor"))
        .def(bp::init<Vector3,Vector3>
             ((bp::arg("linear"),bp::arg("angular")),
              "Initialize from linear and angular components of a Wrench vector (don't mix the order)."))
        .def(bp::init<Vector6>((bp::arg("Vector 6d")),"Init from a vector 6 [force,torque]"))
        .def(bp::init<Force>((bp::arg("other")),"Copy constructor."))
        
        .add_property("linear",
                      &ForcePythonVisitor::getLinear,
                      &ForcePythonVisitor::setLinear,
                      "Linear part of a *this, corresponding to the linear velocity in case of a Spatial velocity.")
        .add_property("angular",
                      &ForcePythonVisitor::getAngular,
                      &ForcePythonVisitor::setAngular,
                      "Angular part of a *this, corresponding to the angular velocity in case of a Spatial velocity.")
        .add_property("vector",
                      &ForcePythonVisitor::getVector,
                      &ForcePythonVisitor::setVector,
                      "Returns the components of *this as a 6d vector.")
        .add_property("np",&ForcePythonVisitor::getVector)
        
        .def("se3Action",&Force::template se3Action<Scalar,Options>,
             bp::args("M"),"Returns the result of the dual action of M on *this.")
        .def("se3ActionInverse",&Force::template se3ActionInverse<Scalar,Options>,
             bp::args("M"),"Returns the result of the dual action of the inverse of M on *this.")
        
        .def("setZero",&ForcePythonVisitor::setZero,
             "Set the linear and angular components of *this to zero.")
        .def("setRandom",&ForcePythonVisitor::setRandom,
             "Set the linear and angular components of *this to random values.")
        
        .def(bp::self + bp::self)
        .def(bp::self += bp::self)
        .def(bp::self - bp::self)
        .def(bp::self -= bp::self)
        .def(-bp::self)
        
        .def(bp::self == bp::self)
        .def(bp::self != bp::self)
        
        .def(bp::self * Scalar())
        .def(Scalar() * bp::self)
        .def(bp::self / Scalar())
        
        .def("isApprox",
             &call_isApprox<Force>::run,
             isApproxForce_overload(bp::args("other","prec"),
                                     "Returns true if *this is approximately equal to other, within the precision given by prec."))
        
        .def("Random",&Force::Random,"Returns a random Force.")
        .staticmethod("Random")
        .def("Zero",&Force::Zero,"Returns a zero Force.")
        .staticmethod("Zero")
        
        .def_pickle(Pickle())
        ;
      }
      
      static void expose()
      {
        bp::class_<Force>("Force",
                          "Force vectors, in se3* == F^6.\n\n"
                          "Supported operations ...",
                          bp::init<>())
        .def(ForcePythonVisitor<Force>())
        .def(CopyableVisitor<Force>())
        .def(PrintableVisitor<Force>())
        ;
        
      }
      
    private:
      
      struct Pickle : bp::pickle_suite
      {
        static
        boost::python::tuple
        getinitargs(const Force & f)
        { return bp::make_tuple((Vector3)f.linear(),(Vector3)f.angular()); }
      };
      
      static Vector3 getLinear(const Force & self ) { return self.linear(); }
      static void setLinear(Force & self, const Vector3 & f) { self.linear(f); }
      static Vector3 getAngular(const Force & self) { return self.angular(); }
      static void setAngular(Force & self, const Vector3 & n) { self.angular(n); }
      
      static void setZero(Force & self) { self.setZero(); }
      static void setRandom(Force & self) { self.setRandom(); }
      
      static Vector6 getVector(const Force & self) { return self.toVector(); }
      static void setVector(Force & self, const Vector6 & f) { self = f; }
    };
    
  } // namespace python
} // namespace pinocchio

#endif // ifndef __pinocchio_python_se3_hpp__

