#include "..\Barcode1.h"
#include "..\..\..\common\ruby_helpers.h"

#include "logging/RhoLog.h"
#undef DEFAULT_LOGCATEGORY
#define DEFAULT_LOGCATEGORY "Barcode1"

#include "ext/rho/rhoruby.h"

static rho::Hashtable<rho::String,IBarcode1*> g_hashBarcodes;
rho::String CBarcode1::m_strDefaultID;

template <typename OBJTYPE, typename FUNCTYPE, typename PARAMTYPE>
static void rho_callObjInUIThread1( OBJTYPE obj, FUNCTYPE pFunc, PARAMTYPE param )
{
    rho::common::IRhoRunnable* pFunctor = new rho::common::CInstanceClassFunctor1<OBJTYPE, FUNCTYPE, PARAMTYPE>( obj, pFunc, param );
    pFunctor->runObject();
}

VALUE CMethodResult::toRuby()
{
    CHoldRubyValue valHash(rho_ruby_createHash());

    return valHash;
    //return rho_ruby_get_NIL();
}

extern "C"
{

VALUE rb_barcode1_s_enumerate(VALUE klass)
{
    CHoldRubyValue valArray(rho_ruby_create_array());

    rho::Vector<rho::String> arIDs = CBarcode1::enumerate();
    for( int i = 0; i < arIDs.size(); i++ )
    {
        if ( !g_hashBarcodes.containsKey(arIDs[i]) )
        {
            IBarcode1* pObj = CBarcode1::create(arIDs[i]);
            g_hashBarcodes.put(arIDs[i], pObj );
        }

        VALUE valObj = rho_create_object_with_id( klass, arIDs[i].c_str() );
        rho_ruby_add_to_array( valArray, valObj );
    }

    return valArray;
}

static rho::String get_default_id()
{
    rho::String strDefaultID = CBarcode1::getDefaultID();
    if ( strDefaultID.length() == 0 )
    {
        CBarcode1::initDefaultID();
        strDefaultID = CBarcode1::getDefaultID();
    }

    if ( !g_hashBarcodes.containsKey(strDefaultID) )
    {
        IBarcode1* pObj = CBarcode1::create(strDefaultID);
        g_hashBarcodes.put(strDefaultID, pObj );
    }

    return strDefaultID;
}

VALUE rb_barcode1_s_default(VALUE klass)
{
    rho::String strDefaultID = get_default_id();

    return rho_create_object_with_id( klass, strDefaultID.c_str() );
}

VALUE rb_barcode1_s_set_default(VALUE klass, VALUE valObj)
{
    const char* szID = rho_get_object_id( valObj );
    CBarcode1::setDefaultID(szID);

    return rho_ruby_get_NIL();
}

static void getStringArrayFromValue(VALUE val, rho::Vector<rho::String>& res)
{
    //TODO: getStringArrayFromValue
}

static VALUE barcode1_getprops(int argc, VALUE *argv, IBarcode1* pObj)
{
    //Sample how to call method in UI thread
/*    if ( argc >= 1 )
    {
        rho_callObjInUIThread1<IBarcode1*, rho::String (IBarcode1::*)(const rho::String&), rho::String>
            ( pObj, &IBarcode1::getProps, "Test" );
    }
*/
    CMethodResult oRes;
    if ( argc == 0 )
    {
        pObj->getProps(oRes);
    }else if ( argc == 1 )
    {
        if ( rho_ruby_is_string(argv[0]) )
        {
            pObj->getProps(getStringFromValue(argv[0]), oRes);
        }else if ( rho_ruby_is_array(argv[0]) )
        {
            rho::Vector<rho::String> ar;
            getStringArrayFromValue(argv[0], ar);
            pObj->getProps(ar, oRes );
        }else
        {
            rho_ruby_raise_argerror("Type error: argument 1 should be String or Array"); //see SWIG Ruby_Format_TypeError
        }
    }else if ( argc == 2 )
    {
        if ( !rho_ruby_is_string(argv[1]) )
            rho_ruby_raise_argerror("Type error: argument 2 should be String"); //see SWIG Ruby_Format_TypeError

        //call with callback - no return value
        if ( rho_ruby_is_string(argv[0]) )
        {
            oRes.setCallback( getStringFromValue(argv[1]) );
            //TODO: run in UI thread or in thread queue
            pObj->getProps( getStringFromValue(argv[0]), oRes );

        }else if ( rho_ruby_is_array(argv[0]) )
        {
            oRes.setCallback( getStringFromValue(argv[1]) );
            //TODO: run in UI thread or in thread queue
            rho::Vector<rho::String> ar;
            getStringArrayFromValue(argv[0], ar);
            pObj->getProps( ar, oRes );
        }else
        {
            rho_ruby_raise_argerror("Type error: argument 1 should be String or Array"); //see SWIG Ruby_Format_TypeError
        }

    }else
    {
        rho_ruby_raise_argerror("wrong # of arguments(%d for 2)", argc );
    }

    return oRes.toRuby();
}

VALUE rb_barcode1_s_getprops(int argc, VALUE *argv)
{
    rho::String strDefaultID = get_default_id();
    IBarcode1* pObj = g_hashBarcodes[strDefaultID];

    return barcode1_getprops(argc, argv, pObj);
}

VALUE rb_barcode1_getprops(int argc, VALUE *argv, VALUE valObj)
{
    const char* szID = rho_get_object_id( valObj );
    IBarcode1* pObj = g_hashBarcodes[szID];

    return barcode1_getprops(argc, argv, pObj);
}

}