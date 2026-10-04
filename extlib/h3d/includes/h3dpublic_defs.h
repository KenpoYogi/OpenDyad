//Copyright>    H3D software ("H3D")
//Copyright>    Copyright 2026 Siemens Industry Software Inc. (“SISW”).
//Copyright>    
//Copyright>    SISW Engineering Inc. grants to third parties limited permission to 
//Copyright>    use H3D solely in connection with OpenRadioss software. 
//Copyright>      
//Copyright>    H3D IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//Copyright>    INCLUDING, BUT NOT LIMITED TO, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
//Copyright>    A PARTICULAR PURPOSE, AND NONINFRINGEMENT.  IN NO EVENT SHALL SISW ENGINEERING
//Copyright>    INC. OR ITS AFFILIATES BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, 
//Copyright>    WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR
//Copyright>    IN CONNECTION WITH H3D OR THE USE OR OTHER DEALINGS IN H3D.

#pragma once

/* for remedial C support */
#if defined(H3D_DEFINE_BOOL) && !defined(__cplusplus)
/*#include <stdbool.h>*/   /* for C99 when available */
#   define true 1
#   define false 0
    typedef int bool;
#endif


/* include integer types for size */
#if defined(_WIN32)
#   include <stddef.h>
#   include <stdint.h>
#else
#   include <stdint.h>
#endif

#include <stdio.h>                          /* for FILE definition                              */
#include <float.h>                          /* for FLT_MAX definition                           */
#include <limits.h>                         /* for UINT_MAX definition                          */


/**************************/
/*   Basic Definitions    */
/**************************/

#define H3D_FileMode int                    /* file creation mode type                          */
#define H3D_SINGLEFILE 0                    /* standard file creation mode                      */
#define H3D_MULTIFILEBYSIM 1                /* H3D files grouped by simulation index            */
#define H3D_APPEND 4                        /* append data to existing file                     */
#define H3D_NOZLIB 8                        /* file witten with zlib or not                     */
#define H3D_SORT 16                         /* enable intrablock entity sorting                 */
#define H3D_QUANT 32                        /* Quantization enabled                             */
#define H3D_NOSORT 64                       /* disable intrablock entity sorting                */
#define H3D_DEBUG 128                       /* enable debug trace output to H3DMessageFunction  */

#define H3D_DEFAULT_COMPRESSION_LEVEL 7     /* default runtime length encoding level            */

#define H3D_PATH_MAX 1024                   /* maximum file pathname length                     */

#define H3D_ID uint32_t                     /* standard H3D ID type                             */
#define H3D_SIM_IDX int                     /* standard H3D simulation index type               */

#define H3D_NULL_ID 0                       /* invalid ID                                       */
#define H3D_MODEL_SIM_IDX -1                /* simulation index of a model                      */
#define H3D_GENERIC_RESULT_SIM_IDX 0        /* generic index for non-simulation specific data   */

#define H3D_DEFAULT_ASSMPOOL   "Assemblies" /* suggested default assemply poolname              */
#define H3D_DEFAULT_COMPPOOL   "Parts"      /* suggested default component poolname             */
#define H3D_DEFAULT_PRIMPOOL   "Primitives" /* suggested default primitive poolname             */
#define H3D_DEFAULT_ELEMPOOL   "Elements"   /* suggested default general element poolname       */
#define H3D_DEFAULT_ELEMPOOL0D "0D"         /* suggested default 0D element poolname            */
#define H3D_DEFAULT_ELEMPOOL1D "1D"         /* suggested default 1D element poolname            */
#define H3D_DEFAULT_ELEMPOOL2D "2D"         /* suggested default 2D element poolname            */
#define H3D_DEFAULT_ELEMPOOL3D "3D"         /* suggested default 3D element poolname            */
#define H3D_DEFAULT_NODEPOOL   "Nodes"      /* suggested default node poolname                  */
#define H3D_DEFAULT_SYSTEMPOOL "Systems"    /* suggested default coord system poolname          */

#define H3D_DEFAULT_FLEXBODYPOOL "Flexible Body"    /* suggested default Flexbody poolname      */
#define H3D_DEFAULT_FLEXELEMPOOL "Flexbody Elements"/* suggested Flexbody element poolname      */
#define H3D_DEFAULT_FLEXNODEPOOL "Flexbody Nodes"   /* suggested default Flexbody node poolname */

#define H3D_NO_RESULT -FLT_MAX              /* tabular data set place holder                    */
#define H3D_DS_NO_LAYER -1                  /* no specific data set layer flag                  */
#define H3D_DT_DELIMITER ":"                /* data type "scalar group" delimiter               */

#define H3D_DEFAULT_POISSONRATIO 0.3f       /* default Poisson's ratio for tensor stain         */

#define H3D_TEXT_FATXML    "h3dFATXML"      /* H3D Text tag for FATXML data                     */
#define H3D_HV_MODEL_XML   "h3dModelXML"    /* H3D Text tag for HV model state data             */
#define H3D_HV_RESULT_XML  "h3dResultXML"   /* H3D Text tag for HV result state data            */
#define H3D_HV_DISPLAY_XML "h3dDisplayXML"  /* H3D Text tag for HV display state data           */


/**************************/
/*   Model Entity Enums   */
/**************************/
typedef enum _H3D_NodeStorageType 
{
    H3D_NODE_UNSET  = 0,
    H3D_NODE_FLOAT  = 1,
    H3D_NODE_DOUBLE = 2
} H3D_NodeStorageType;

typedef enum _H3D_ElementConfig 
{
    H3D_ELEM_CONFIG_MASS =      1,
    H3D_ELEM_CONFIG_PLOT =      2,
    H3D_ELEM_CONFIG_WELD =      3,
    H3D_ELEM_CONFIG_RIGID =     5,
    H3D_ELEM_CONFIG_SPRING =    21,
    H3D_ELEM_CONFIG_JOINT =     22,
    H3D_ELEM_CONFIG_RIGIDLINK = 55,
    H3D_ELEM_CONFIG_RBE3 =      56,
    H3D_ELEM_CONFIG_BAR =       60,
    H3D_ELEM_CONFIG_ROD =       61,
    H3D_ELEM_CONFIG_BAR3 =      63,
    H3D_ELEM_CONFIG_GAP =       70,
    H3D_ELEM_CONFIG_ACM =       71,
    H3D_ELEM_CONFIG_CWELD =     72,
    H3D_ELEM_CONFIG_TRIA3 =     103,
    H3D_ELEM_CONFIG_QUAD4 =     104,
    H3D_ELEM_CONFIG_TRIA6 =     106,
    H3D_ELEM_CONFIG_QUAD8 =     108,
    H3D_ELEM_CONFIG_MASTER3 =   123,
    H3D_ELEM_CONFIG_MASTER4 =   124,
    H3D_ELEM_CONFIG_SLAVE3 =    133,
    H3D_ELEM_CONFIG_SLAVE4 =    134,
    H3D_ELEM_CONFIG_SLAVE1 =    135,
    H3D_ELEM_CONFIG_TETRA4 =    204,
    H3D_ELEM_CONFIG_PENTA5 =    205,
    H3D_ELEM_CONFIG_PENTA6 =    206,
    H3D_ELEM_CONFIG_HEX8 =      208,
    H3D_ELEM_CONFIG_TETRA10 =   210,
    H3D_ELEM_CONFIG_PENTA12 =   212,
    H3D_ELEM_CONFIG_PENTA13 =   213,
    H3D_ELEM_CONFIG_PENTA15 =   215,
    H3D_ELEM_CONFIG_HEX16 =     216,
    H3D_ELEM_CONFIG_HEX20 =     220
} H3D_ElementConfig;
#define H3D_ElementConfig uint32_t

typedef enum _H3D_ElementStorageType 
{
    H3D_ELEM_UNSET          = 0,
    H3D_ELEM_FORM1          = 1,
    H3D_ELEM_FORM2          = 2,
    H3D_ELEM_FORM3          = 3,
    H3D_ELEM_FORM1_MIXED    = 4,
    H3D_ELEM_FORM2_MIXED    = 5
} H3D_ElementStorageType;

typedef enum _H3D_SYSTEM_TYPE 
{
    H3D_CARTESIAN   = 0,
    H3D_CYLINDRICAL = 1,
    H3D_SPHERICAL   = 2,
    H3D_TRACKING    = 3
} H3D_SYSTEM_TYPE;
#define H3D_SYSTEM_TYPE uint32_t

typedef enum _H3D_SystemStorageType 
{
    H3D_SYS_UNSET          = 0,
    H3D_SYS_FORM1          = 1,
    H3D_SYS_FORM2          = 2
} H3D_SystemStorageType;

typedef enum _H3D_SystemDefMethod 
{
    H3D_SYS_DM_UNSET        = 0,
    H3D_SYS_DM_ORIGIN_XY    = 1,
    H3D_SYS_DM_CIRCLE       = 2
} H3D_SystemDefMethod;

typedef enum _H3D_PRIMITIVE_TYPE
{
    H3D_CYLINDER  = 0,
    H3D_SPRING    = 1,
    H3D_BOX       = 2,
    H3D_MARKER    = 3,
    H3D_LINE      = 5,
    H3D_ELLIPSOID = 8,
    H3D_MESH      = 11,
    H3D_ROAD      = 12,
    H3D_PRISM     = 14,
    H3D_SOLID     = 15,
    H3D_EXTOBJECT = 16,
    H3D_OUTLINE   = 53
} H3D_PRIMITIVE_TYPE;

typedef struct _H3D_BoxParameters
{
    H3D_PRIMITIVE_TYPE type;
    float              xsize;
    float              ysize;
    float              zsize;
    bool               centered;
} H3D_BoxParameters;

typedef struct _H3D_MarkerParameters
{
    H3D_PRIMITIVE_TYPE type;
} H3D_MarkerParameters ;

typedef enum _H3D_PRIMITIVE_CAPS 
{ 
    NOCAPS,
    RIGHTCAP, 
    LEFTCAP, 
    BOTHCAPS 
} H3D_PRIMITIVE_CAPS;

typedef struct _H3D_CylinderParameters
{
    H3D_PRIMITIVE_TYPE type;
    int                angle;
    int                segments_in_yz;
    float              sweep_angle;
    H3D_PRIMITIVE_CAPS cap;
    float              semi_x;
    float              semi_y1; 
    float              semi_y2;
    float              semi_z1;
    float              semi_z2;
    bool               centered;
} H3D_CylinderParameters;

typedef struct _H3D_EllipsoidParameters
{
    H3D_PRIMITIVE_TYPE type;
    int                angle;
    int                segments_in_z;
    int                segments_in_xy;
    float              semi_x;
    float              semi_y;
    float              semi_z;
} H3D_EllipsoidParameters;

typedef struct _H3D_MeshParameters
{
    H3D_PRIMITIVE_TYPE type;
    uint32_t           tconn_length;
    uint32_t           lconn_length;
    uint32_t           num_nodes;
} H3D_MeshParameters;

typedef struct _H3D_OutlineParameters
{
    H3D_PRIMITIVE_TYPE type;
    uint32_t           num_markers;
} H3D_OutlineParameters;

typedef struct _H3D_SpringParameters
{
    H3D_PRIMITIVE_TYPE type;
    H3D_ID             marker1_id;
    H3D_ID             marker1_pname_id;
    H3D_ID             marker2_id;
    H3D_ID             marker2_pname_id;
    float              offset_marker1_damper;
    float              offset_marker2_damper;
    float              height_damper1;
    float              height_damper2;
    float              diameter_spring;
    float              diameter_damper1;
    float              diameter_damper2;
    int32_t            helix_coils;
    int32_t            helix_segs;
} H3D_SpringParameters;

typedef enum _H3D_FLEX_TYPE 
{
    FLX_UNSPECIFIED         = -1,
    FLX_EIGENVAL            = 0,        /* double */
    FLX_EIGENVEC_retired    = 1,        /* double */
    FLX_NODAMASS            = 2,        /* double */
    FLX_NODINERT_retired    = 3,        /* double */
    FLX_DISPMODE            = 4,        /* float vector by block */
    FLX_ROTAMODE            = 5,        /* float vector by block */
    FLX_ESTRESMOD           = 6,        /* float tensor by block */
    FLX_INTERNOD_retired    = 7,        /* unsigned int */
    FLX_ESTRESTYP           = 8,        /* unsigned int */
    FLX_INERTNID_retired    = 9,        /* unsigned int */
    FLX_INTERNID            = 10,       /* unsigned int */
    FLX_COUPMASS_retired    = 11,       /* double */
    FLX_STIFFMTX_retired    = 12,       /* double */
    FLX_DIFSTIFF_retired    = 13,       /* double */
    FLX_GMASS_retired       = 14,       /* double */
    FLX_GCMCOORD_retired    = 15,       /* double */
    FLX_GINERTIA_retired    = 16,       /* double */
    FLX_INVARS              = 17,       /* double */
    FLX_NSTRESTYP           = 18,       /* unsigned int */
    FLX_NSTRESMOD           = 19,       /* float tensor by block */
    FLX_NSTRNTYP            = 20,       /* unsigned int */
    FLX_NSTRNMOD            = 21,       /* float tensor by block */
    FLX_GCMLOC_retired      = 22,       /* double */
    FLX_GCMORIENT_retired   = 23,       /* double */
    FLX_GLPRFLOC_retired    = 24,       /* double */
    FLX_GLPRFORIENT_retired = 25,       /* double */
    FLX_INVARS_OSM          = 26,       /* double */
    FLX_ESTRESSET_retired   = 27,       /* unsigned int */
    FLX_ESTRAINMOD          = 28,       /* float tensor by block */
    FLX_ESTRAINTYP          = 29,       /* float */
    FLX_ESTRAINSET_retired  = 30,       /* unsigned int */
    FLX_NSTRES_NSET_retired = 31,       /* unsigned int */
    FLX_NSTRES_MSET         = 32,       /* unsigned int */
    FLX_NSTRN_NSET_retired  = 33,       /* unsigned int */
    FLX_NSTRN_MSET          = 34,       /* unsigned int */
    FLX_GEOSTIFF            = 35,       /* float by block */
    FLX_NODEHESSSET_retired = 36,       /* unsigned int */
    FLX_NODEHESSIAN         = 37,       /* float by block */
    FLX_ISNODAL             = 38,       /* unsigned int */
    FLX_NODALSET            = 39,       /* unsigned int */
    FLX_NODALKMTX           = 40,       /* double by block */
    FLX_NODALMMTX           = 41,       /* double by block */
    FLX_DAMPINGMTX          = 42,       /* double by block */
    FLX_STRUCTDAMP          = 43,       /* double by block */
    FLX_LOADVECTOR          = 44,       /* double by block */
    FLX_ASETCOORD           = 45,       /* double */
    FLX_DMIGCORD            = 46,       /* double */
    FLX_FLUIDSET            = 47,       /* unsigned int */
    FLX_FLUIDKMTX           = 48,       /* float by block */
    FLX_FLUIDMMTX           = 49,       /* float by block */
    FLX_FLUIDDAMP           = 50,       /* double by block */
    FLX_FLUIDAMTX           = 51,       /* double by block */
    FLX_ASETPCOORD          = 52,       /* double */
    FLX_GRIDCOORD           = 53,       /* double */
    FLX_STRUCT_CSET         = 54,       /* unsigned int */
    FLX_FLUID_CSET          = 55,       /* unsigned int */
    FLX_STRUCT_MODE         = 56,       /* double by block */
    FLX_FLUID_MODE          = 57,       /* double by block */
    FLX_MODELUNITS          = 58,       /* unsigned int */
    FLX_SHELL_THICKNESS     = 59        /* float */
} H3D_FLEX_TYPE;
#define H3D_FLEX_TYPE int
#define H3D_MAX_FLEX_TYPES 60           /* max number of types per body     */


typedef enum _H3D_FLEX_UNITS
{
    H3D_FLEX_UNITLESS       =  0,       /* assumed to be of the same units as the model */
    H3D_FLEX_DEFAULT        =  1<<1,    /* kg-N-mm-sec units                            */
    H3D_FLEX_USER_DEFINED   =  1<<2     /* For future implementation                    */
} H3D_FLEX_UNITS;

/* HV/HG will read types 4,5,6 & 11, plus any type in the from 12 to 50.                */
/* Matrix types not intended to be read by HV/HG must have a value greater than 50!     */
typedef enum _H3D_MTX_TYPE
{
    H3D_MTX_UNKNOWN          =  0,      /* general purpose none                         */
    H3D_MTX_FSINTERFACE      =  1,      /* Fluid-Structure interface matrix             */
    H3D_MTX_FSSTRUCTMODE     =  2,      /* Structural modes at the interface DOFs       */
    H3D_MTX_FSFLUIDMODE      =  3,      /* Fluid modes at the interface DOFs            */
    H3D_MTX_STRUCTPFMODE     =  4,      /* Complex struct component modal participation */
    H3D_MTX_FLUIDPFMODE      =  5,      /* Complex fluid component modal participation  */
    H3D_MTX_FSMODE           =  6,      /* Complex structural CMS for fluid response    */
    H3D_MTX_FLUIDMODES       =  7,      /* Fluid modes                                  */
    H3D_MTX_CDSCOMMATRIX     =  8,      /* CDS complex matrix                           */
    H3D_MTX_CDSINTERDOF      =  9,      /* CDS interior DOFs                            */
    H3D_MTX_CDSRECOVERMATRIX = 10,      /* CDS interior recovery matrix                 */
    H3D_MTX_DSANVH           = 11,      /* Complex sensitivity output                   */
    H3D_MTX_REACTIONFORCE    = 12,      /* Reaction force at the connection points      */
    H3D_MTX_TRANSFERFUNCTION = 13,      /* Transfer function for a response DOF         */
    H3D_MTX_TPADERIVEDRESP   = 14
} H3D_MTX_TYPE;

typedef enum _H3D_MTX_FORMAT
{
    H3D_MTX_UNSPECIFIED    =  0,       /* general purpose none              */
    H3D_MTX_GEN            =  1<<1,    /* general dense matrix              */
    H3D_MTX_SYM            =  1<<2,    /* symmetric dense matrix            */
    H3D_MTX_GEN_SPARSE     =  1<<3,    /* general sparse matrix             */
    H3D_MTX_SYM_SPARSE     =  1<<4     /* symmetric sparse matrix           */
} H3D_MTX_FORMAT;

typedef enum _H3D_MTX_VALUE_FORMAT
{
    H3D_MTX_UNSET          =  0,       /* general purpose none              */
    H3D_MTX_REAL8          =  1<<1,    /* double                            */
    H3D_MTX_INT8           =  1<<2,    /* uint64_t                          */
    H3D_MTX_REAL4          =  1<<3,    /* float                             */
    H3D_MTX_INT4           =  1<<4,    /* int                               */
    H3D_MTX_REAL8_COMPLEX  =  1<<5,    /* double complex                    */
    H3D_MTX_REAL4_COMPLEX  =  1<<6     /* float complex                     */
} H3D_MTX_VALUE_FORMAT;

typedef enum _H3D_MODAL_TYPE
{
    H3D_MODT_UNSET          =  0,       /* general purpose none              */
    H3D_MODT_STRUCTURE      =  1<<1,    
    H3D_MODT_FLUID          =  1<<2,    
    H3D_MODT_RESIDUAL       =  1<<3,    
    H3D_MODT_RIGIDBODY      =  1<<4     
} H3D_MODAL_TYPE;

typedef enum _H3D_PROP_ISOP
{ 
    H3D_PROP_ISOP_UNSET     =  0,       /* general purpose none              */
    H3D_PROP_ISOP_FULL      =  1<<1,    
    H3D_PROP_ISOP_MODPLAST  =  1<<2,    
    H3D_PROP_ISOP_REDPLAST  =  1<<3,    
    H3D_PROP_ISOP_INT0      =  1<<4     
} H3D_PROP_ISOP;

typedef enum _H3D_PROP_FCTN
{
    H3D_PROP_FCTN_UNSET     =  0,       /* general purpose none              */
    H3D_PROP_FCTN_SMECH     =  1<<1,    
    H3D_PROP_FCTN_PFLUID    =  1<<2,    
    H3D_PROP_FCTN_PORO      =  1<<3,    
} H3D_PROP_FCTN;


/*******************************/
/*   Model Entity Attributes   */
/*******************************/

typedef enum _H3D_MODEL_ATTRIBS
{
    H3D_MODEL_ATTR_NONE               =  0,     /* general purpose none                   */
    H3D_MODEL_ATTR_LOC                =  1<<1,  /* model locator data                     */
    H3D_MODEL_ATTR_SYNC_MODE          =  1<<2,  /* Hv Display Sync Mode Flag              */
    H3D_MODEL_ATTR_GRAD_COLOR         =  1<<3,  /* Hv Display Apply Gradient Color        */
    H3D_MODEL_ATTR_COLOR_PART         =  1<<4,  /* Hv Display Color By Part               */
    H3D_MODEL_ATTR_COLOR_MODEL        =  1<<5,  /* Hv Display Color By Model              */
    H3D_MODEL_ATTR_COLOR_CONFIG       =  1<<6,  /* Hv Display Color By Element Config     */
    H3D_MODEL_ATTR_COLOR_DIMENSION    =  1<<7,  /* Hv Display Color By Element Dimension  */
    H3D_MODEL_ATTR_COLOR_NORMAL       =  1<<8,  /* Hv Display Color By Normal             */
    H3D_MODEL_ATTR_COLOR_PARTASSEMBLY =  1<<9   /* Hv Display Color By Part Assembly      */

} H3D_MODEL_ATTRIBS;

typedef enum _H3D_ASSEMBLY_ATTRIBS
{
    H3D_ASSM_ATTR_NONE         =  0,        /* general purpose none                    */
    H3D_ASSM_ATTR_ID           =  1<<5      /* assembly ids on                         */
} H3D_ASSEMBLY_ATTRIBS;

typedef enum _H3D_COMPONENT_ATTRIBS
{
    H3D_COMP_ATTR_NONE          =  0,       /* general purpose none                    */
/* retired flags (obsolete H3D11) */
    H3D_COMP_ATTR_VISIBLE       =  1<<0,    /* visibility on        (obsolete H3D11)   */
    H3D_COMP_ATTR_BLEND         =  1<<3,    /* transparent polygons (obsolete H3D11)   */
    H3D_COMP_ATTR_POLYGON       =  1<<4,    /* polygon on           (obsolete H3D11)   */
    H3D_COMP_ATTR_INFIT         =  1<<8,    /* use in fit to screen (obsolete H3D11)   */
    H3D_COMP_ATTR_INCUT         =  1<<11,   /* use in cut operation (obsolete H3D11)   */
    H3D_COMP_ATTR_INISO         =  1<<12,   /* use in iso operation (obsolete H3D11)   */

/* component related display objects */
    H3D_COMP_ATTR_ID            =  1<<5,    /* component ids on                        */
    H3D_COMP_ATTR_BBOX          =  1<<6,    /* component bounding box on               */
    H3D_COMP_ATTR_HIGHLIGHT     =  1<<7,    /* component highlighted                   */

/* component mesh display mode attributes */
    H3D_COMP_ATTR_MESHLINES     =  1<<13,   /* display all mesh lines                  */
    H3D_COMP_ATTR_FEATURES      =  1<<1,    /* display only feature lines              */
    H3D_COMP_ATTR_EDGES         =  1<<2,    /* display only edge lines                 */

/* component mesh opacity mode attributes */
    H3D_COMP_ATTR_TRANSPARENT   =  1<<16,   /* display mesh with tranparent elements   */
    H3D_COMP_ATTR_OPAQUE        =  1<<17,   /* display mesh with opeque elements       */
    H3D_COMP_ATTR_WIREFRAME     =  1<<18,   /* display wireframe mesh only             */

/* component lighting attributes */
    H3D_COMP_ATTR_LIGHT_FLAT    =  1<<9,    /* flat shading used to render elements    */
    H3D_COMP_ATTR_LIGHT_SMOOTH  =  1<<10,   /* smooth shading used to render elements  */

/* component usage attributes */
    H3D_COMP_ATTR_NOFIT         =  1<<19,   /* don't use in fit to screen operation    */
    H3D_COMP_ATTR_NOCUT         =  1<<20,   /* don't use in cut operation              */
    H3D_COMP_ATTR_NOISO         =  1<<21    /* don't use in iso operation              */
} H3D_COMPONENT_ATTRIBS ;

typedef enum _H3D_SYSTEM_ATTRIBS
{
    H3D_SYS_ATTR_NONE           =  0,       /* general purpose none                     */
    H3D_SYS_ATTR_VISIBLE        =  1<<0,    /* system visibility (obsolete H3D11)       */
    H3D_SYS_ATTR_ID             =  1<<5,    /* system ids on                            */
    H3D_SYS_ATTR_BBOX           =  1<<6,    /* system bounding box on                   */
    H3D_SYS_ATTR_HIGHLIGHT      =  1<<7,    /* system highlighted                       */

    H3D_SYS_ATTR_USER_DEFINED   =  1<<9,    /* system added as post-solver data         */

/* component usage attributes */
    H3D_SYS_ATTR_NOFIT          =  1<<19,    /* don't use in fit to screen operation    */
    H3D_SYS_ATTR_NOCUT          =  1<<20,    /* don't use in cut operation              */
    H3D_SYS_ATTR_NOISO          =  1<<21     /* don't use in iso operation              */
} H3D_SYSTEM_ATTRIBS;

typedef enum _H3D_ENTITY_SET_ATTRIBS
{
    H3D_ENT_SET_ATTR_NONE       =  0,       /* general purpose none                     */
    H3D_ENT_SET_ATTR_VISIBLE    =  1<<0,    /* set visibility (obsolete H3D11)          */
    H3D_ENT_SET_ATTR_ID         =  1<<5,    /* set ids on                               */
    H3D_ENT_SET_ATTR_POINT      =  1<<1,    /* set marker as point on                   */
    H3D_ENT_SET_ATTR_SPHERE     =  1<<2,    /* set marker as sphere on                  */
    H3D_ENT_SET_ATTR_WIRE       =  1<<3,    /* set marker as wireframe on               */
    H3D_ENT_SET_ATTR_SHADED     =  1<<4     /* set marker as shading on                 */
} H3D_ENTITY_SET_ATTRIBS;

typedef enum _H3D_IMAGE_TYPE
{
    H3D_IMAGE_UNKNOWN       = 0,
    H3D_IMAGE_PREVIEW       = 1<<0,
    H3D_IMAGE_JPEG          = 1<<1,
    H3D_IMAGE_PNG           = 1<<2,
    H3D_IMAGE_OVERLAY       = 1<<3
} H3D_IMAGE_TYPE;

typedef enum _H3D_VIEW_TYPE
{
    H3D_VIEW_UNKNOWN    = 0,
    H3D_VIEW_ORTHO      = 1,
    H3D_VIEW_PERSPECT   = 2,
    H3D_VIEW_LENS       = 3
} H3D_VIEW_TYPE;


/***************************/
/*   Result Entity Enums   */
/***************************/

typedef enum _H3D_DS_SHELL_METHOD
{
    H3D_SM_UNKNOWN  = 0,
    H3D_SM_BISECT   = 1, 
    H3D_SM_N1N2     = 2,
    H3D_SM_MIDPOINT = 3
} H3D_DS_SHELL_METHOD;

typedef enum _H3D_ID_POOL_TYPE 
{ 
    H3D_POOL_UNKNOWN   = 0, 
    H3D_POOL_NODE      = 1, 
    H3D_POOL_ELEMENT   = 2, 
    H3D_POOL_COMPONENT = 3, 
    H3D_POOL_SYSTEM    = 4, 
    H3D_POOL_ASSEMBLY  = 5,
    H3D_POOL_MODEL     = 6,
    H3D_POOL_SET       = 7,
    H3D_POOL_MARKER    = 8
} H3D_ID_POOL_TYPE;


typedef enum _H3D_ANALYSIS_TYPE
{
    H3D_ANALYSIS_UNSET          = 0,
    H3D_LINEAR_REAL             = 1,    /* HV small displacements (real), shell coordinate systems don't deform    */
    H3D_LINEAR_COMPLEX          = 2,    /* HV small displacements (complex), shell coordinate systems don't deform */
    H3D_NONLINEAR               = 3,    /* HV large displacements (real), shell coordinate systems deform          */
    H3D_OS_STATICS              = 4,    /* LINEAR_REAL          */
    H3D_OS_NLSTAT               = 5,    /* NONLINEAR            */
    H3D_OS_MODES                = 6,    /* LINEAR_REAL          */
    H3D_OS_MCEIG                = 7,    /* LINEAR_COMPLEX       */
    H3D_OS_BUCK                 = 8,    /* LINEAR_REAL          */
    H3D_OS_DFREQ                = 9,    /* LINEAR_COMPLEX       */
    H3D_OS_MFREQ                = 10,   /* LINEAR_COMPLEX       */
    H3D_OS_DTRAN_LINEAR         = 11,   /* NONLINEAR            */
    H3D_OS_DTRAN_NONLINEAR      = 12,   /* NONLINEAR            */
    H3D_OS_MTRAN                = 13,   /* NONLINEAR            */
    H3D_OS_DFOUR_TIME           = 14,   /* NONLINEAR            */
    H3D_OS_DFOUR_FREQ           = 15,   /* LINEAR_COMPLEX       */
    H3D_OS_MFOUR_TIME           = 16,   /* NONLINEAR            */
    H3D_OS_MFOUR_FREQ           = 17,   /* LINEAR_COMPLEX       */
    H3D_OS_HEAT_STEADY          = 18,   /* LINEAR_REAL          */
    H3D_OS_HEAT_TRANSIENT       = 19,   /* NONLINEAR            */
    H3D_OS_NLHEAT_STEADY        = 20,   /* NONLINEAR            */
    H3D_OS_NLHEAT_TRANSIENT     = 21,   /* NONLINEAR            */
    H3D_OS_MBD                  = 22,   /* NONLINEAR            */
    H3D_OS_EXPDYN               = 23,   /* NONLINEAR            */
    H3D_OS_FATIGUE              = 24,   /* LINEAR_REAL          */
    H3D_OS_RANDOM               = 25,   /* LINEAR_REAL          */
    H3D_OS_NLEXPL               = 26,   /* NONLINEAR            */
    H3D_OS_RSPEC                = 27,   /* LINEAR_REAL          */
    H3D_OS_DCEIG                = 28,   /* LINEAR_COMPLEX       */
    H3D_OS_CMSMETH              = 29
} H3D_ANALYSIS_TYPE;

typedef enum _H3D_NODAL_DATA_TYPE 
{
    H3D_NODAL_UNKNOWN       = 0,
    H3D_NODAL_POSITION      = 1,
    H3D_NODAL_DISPLACEMENT  = 2,
    H3D_NODAL_RELATIVE_DISP = 3
} H3D_NODAL_DATA_TYPE;

typedef enum _H3D_SUBCASE_TYPE 
{
    H3D_SUBCASE_UNKNOWN       = 0,
    H3D_SUBCASE_BASIC         = 1,
    H3D_SUBCASE_SUPERPOSITION = 2,  /* (Linear) Superposition           */
    H3D_SUBCASE_ENVELOPE_MIN  = 3,  /* Envelope with MIN function       */
    H3D_SUBCASE_ENVELOPE_MAX  = 4,  /* Envelope with MAX function       */
    H3D_SUBCASE_ENVELOPE_EXT  = 5,  /* Envelope with EXTREME function   */
    H3D_SUBCASE_ENVELOPE_RNG  = 6   /* Envelope with RANGE function     */
} H3D_SUBCASE_TYPE;


typedef enum _H3D_DS_TYPE
{
    H3D_DS_UNKNOWN      = UINT_MAX,
    H3D_DS_NODE         = 0,
    H3D_DS_ELEM         = 1, 
    H3D_DS_SURFACE      = 3,
    H3D_DS_VOLUME       = 4,
    H3D_DS_PART         = 5,
    H3D_DS_MARKER       = 6,
    H3D_DS_ASSEMBLY     = 7,
    H3D_DS_SET          = 8,
    H3D_DS_MODEL        = 9,
    H3D_DS_SYSTEM       = 10
} H3D_DS_TYPE;

typedef enum _H3D_DS_FORMAT
{
    H3D_DS_NONE              = 0,   /* indicates an unknown or non-existent format      */
    H3D_DS_SCALAR            = 1,   /* 1 float  per item                                */
    H3D_DS_EROSION           = 17,  /* 1 float  per item                                */
    H3D_DS_RADIUS            = 22,  /* 1 float  per item                                */
    H3D_DS_VECTOR            = 2,   /* 3 floats per item                                */ 
    H3D_DS_TENSOR2D          = 15,  /* 3 floats per item (sxx, syy, sxy)                */
    H3D_DS_TENSOR3D          = 3,   /* 6 floats per item (sxx, syy, szz, sxy, syz, szx) */
    H3D_DS_TRANSFORM         = 4,   /* 6 floats per item (x, y, z, e1, e2, e3)          */
    H3D_DS_MODAL_FACTORS     = 5,   /* #modes floats per item                           */
    H3D_DS_FLEX_DISPLACEMENT = 6,   /* 0 dataset must be calculated                     */
    H3D_DS_FLEX_ROTATION     = 7,   /* 0 dataset must be calculated                     */
    H3D_DS_FLEX_ESTRESS      = 8,   /* 0 dataset must be calculated                     */
    H3D_DS_MBDTENSOR         = 9,   /* 6 floats per item                                */
    H3D_DS_FLEX_ESTRAIN      = 10,  /* 0 dataset must be calculated                     */
    H3D_DS_FLEX_NSTRESS      = 11,  /* 0 dataset must be calculated                     */
    H3D_DS_FLEX_NSTRAIN      = 12,  /* 0 dataset must be calculated                     */
    H3D_DS_FLEX_ACCELERATION = 13,  /* 0 dataset must be calculated                     */
    H3D_DS_FLEX_VELOCITY     = 14,  /* 0 dataset must be calculated                     */
    H3D_DS_ORIENT_VECTOR     = 16,
    H3D_DS_SCALAR_DOUBLE     = 18,  /* 1 double  per item                               */
    H3D_DS_VECTOR_DOUBLE     = 19,  /* 3 doubles  per item                              */
    H3D_DS_TENSOR2D_DOUBLE   = 20,  /* 3 doubles  per item                              */
    H3D_DS_TENSOR3D_DOUBLE   = 21,  /* 6 doubles  per item                              */
    H3D_DS_FLEX_THICKNESS    = 23   /* 0 dataset must extracted from flex body          */
} H3D_DS_FORMAT;

typedef enum _H3D_NF_FORMAT
{
    H3D_NF_UNSET            = 0,
    H3D_NF_REAL             = 1, 
    H3D_NF_COMPLEX_CART     = 2,    /* complex retangular (real/imaginary) format       */
    H3D_NF_COMPLEX_POLAR    = 3     /* complex polar (magnitude/phase) format           */
} H3D_NF_FORMAT;

typedef enum _H3D_ANALYSIS_SYSTEM
{
    H3D_DS_UNSET    = INT_MIN,
    H3D_DS_PLY      = -3,
    H3D_DS_MATERIAL = -2,
    H3D_DS_ANALYSIS = -1,
    H3D_DS_GLOBAL   =  0, 
    H3D_DS_BY_ID    =  1
} H3D_ANALYSIS_SYSTEM;

typedef enum _H3D_TENSOR_TYPE
{
    H3D_DS_STRESS     = 0,
    H3D_DS_STRAIN     = 1<<0,
    H3D_DS_STRESS_2D  = 1<<1,
    H3D_DS_STRAIN_2D  = 1<<2,
    H3D_DS_ESTRAIN    = 1<<3,
    H3D_DS_ESTRAIN_2D = 1<<4
} H3D_TENSOR_TYPE;


/* forward declaration */
typedef struct _H3DFileInfo { 
    void* data; 
    void* client_data1;
    void* client_data2;
} H3DFileInfo; 

typedef struct _H3DReaderFunctionTable H3DReaderFunctionTable;

typedef void (*H3DErrorFunctionType)(H3DFileInfo* context, const char* errorText);
typedef void (*H3DMessageFunctionType)(H3DFileInfo* context, const char* messageText);

typedef enum _H3D_TRIBOOL
{
    H3D_BOOL_UNSET = -1,
    H3D_BOOL_FALSE = 0,
    H3D_BOOL_TRUE  = 1
} H3D_TRIBOOL;
