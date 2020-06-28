
#ifndef __mast_libmesh_fe_side_data_h__
#define __mast_libmesh_fe_side_data_h__

// MAST includes
#include <mast/quadrature/libmesh/quadrature.hpp>
#include <mast/fe/libmesh/fe.hpp>


namespace MAST {
namespace FEBasis {
namespace libMeshWrapper {

template <uint_t Dim,
          typename FEBasisType,
          typename FEDerivativeType>
class FESideData {
    
public:
        
    // libMesh quadrature and fe types are implemented for real variables only
    using quadrature_t = typename MAST::Quadrature::libMeshWrapper::Quadrature<real_t, Dim-1>;
    using fe_basis_t   = FEBasisType;
    using scalar_t     = typename FEDerivativeType::scalar_t;
    static_assert(std::is_same<FEBasisType, MAST::FEBasis::libMeshWrapper::FEBasis<real_t, Dim>>::value,
                  "FEBasisType should be libMeshWrapper::FEBasis.");
    static_assert(std::is_same<FEBasisType, typename FEDerivativeType::fe_basis_t>::value,
                  "Different FEBasisType than that used for instantiation of FEDerivativeType.");
    static_assert(std::is_same<typename FEDerivativeType::basis_scalar_t, real_t>::value,
                  "basis_scalar_t for provided FEDerivativeType must be real_t.");

    FESideData():
    _initialized (false),
    _q           (nullptr),
    _fe_basis    (nullptr),
    _fe_deriv    (nullptr)
    { }
    
    virtual ~FESideData() {
        
        if (_q)         delete _q;
        if (_fe_basis)  delete _fe_basis;
        if (_fe_deriv)  delete _fe_deriv;
    }


    inline void init (libMesh::Order          q_order,
                      libMesh::QuadratureType q_type,
                      libMesh::Order          fe_order,
                      libMesh::FEFamily       fe_type) {
        
        Assert0(!_initialized, "Object already initialized");
        
        _q        = new quadrature_t(q_type, q_order);
        _fe_basis = new fe_basis_t(libMesh::FEType(fe_order, fe_type));
        _fe_deriv = new FEDerivativeType;
        
        _fe_deriv->set_fe_basis(*_fe_basis);
        
        _initialized = true;
    }

    
    template <typename ContextType>
    inline void reinit_for_side(const ContextType& c, uint_t s) {
        
        _fe_basis->reinit_for_side(*c.elem, *_q, s);
        _fe_deriv->reinit_for_side(c);
    }
    
private:
    
    bool               _initialized;
    quadrature_t      *_q;
    fe_basis_t        *_fe_basis;
    FEDerivativeType  *_fe_deriv;
};

} // namespace libMeshWrapper
} // namespace FEBasis
} // namespace MAST

#endif // __mast_libmesh_fe_side_data_h__