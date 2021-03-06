/*
* MAST: Multidisciplinary-design Adaptation and Sensitivity Toolkit
* Copyright (C) 2013-2020  Manav Bhatia and MAST authors
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Catch includes
#include "catch.hpp"

// MAST includes
#include <mast/optimization/topology/simp/penalized_density.hpp>

// Test includes
#include <test_helpers.h>

namespace MAST {
namespace Test {
namespace Optimization {
namespace Topology {
namespace SIMP {

struct Context {};

template <typename ScalarType>
struct DensityField {
    
    DensityField(): v(0.5), dv(0.75) {}
    
    inline ScalarType
    value(const Context& c) const { return v;}
    
    template <typename ScalarFieldType>
    inline ScalarType
    derivative(const Context& c, const ScalarFieldType& f) const
    {
        return dv;
    }
    
    ScalarType v;
    real_t     dv;
};

inline void test_penalized_density_sensitivity()  {

    
    Context c;
    DensityField<real_t> field;
    MAST::Optimization::Topology::SIMP::PenalizedDensity
    <real_t, DensityField<real_t>>
    density;

    density.set_density_field(field);
    density.set_penalty(3.);
    
    real_t
    rho     = density.value(c),
    drho    = density.derivative(c, field),
    drho_cs = 0.,
    drho_ad = 0.;
    
    // complex step sensitivity
    {
        DensityField<complex_t> field_cs;
        field_cs.v += complex_t(0., ComplexStepDelta);
        
        MAST::Optimization::Topology::SIMP::PenalizedDensity
        <complex_t, DensityField<complex_t>>
        density_cs;

        density_cs.set_density_field(field_cs);
        density_cs.set_penalty(3.);
        
        complex_t
        rho_cs  = density_cs.value(c);

        drho_cs = field_cs.dv * rho_cs.imag()/ComplexStepDelta;

        CHECK(drho == Catch::Detail::Approx(drho_cs));
    }

#if MAST_ENABLE_ADOLC == 1
    // automatic differentiation
    {
        adtl::setNumDir(1);

        DensityField<adouble_tl_t> field_ad;
        field_ad.v.setADValue(&field_ad.dv);
        
        MAST::Optimization::Topology::SIMP::PenalizedDensity
        <adouble_tl_t, DensityField<adouble_tl_t>>
        density_ad;

        density_ad.set_density_field(field_ad);
        density_ad.set_penalty(3.);
        
        adouble_tl_t
        rho_ad  = density_ad.value(c);

        drho_ad = *rho_ad.getADValue();

        CHECK(drho == Catch::Detail::Approx(drho_ad));
    }
#endif
}



TEST_CASE("penalized_density_sensitivity",
          "[Optimization][Topology][SIMP][ComplexStep][AdolC]") {
    
    test_penalized_density_sensitivity();
}

} // namespace SIMP
} // namespace Topology
} // namespace Optimization
} // namespace Test
} // namespace MAST


