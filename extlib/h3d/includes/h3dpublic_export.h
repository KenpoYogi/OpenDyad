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

/* H3D API Lexicon:                                                          */
/*      model           -  a collection of assemblies, components,           */
/*                          primitives, elements, positional and simulation  */ 
/*                          data                                             */

/*      assembly        - a grouping of components that will be              */
/*                          reorientation as a group                         */
/*      component       - a collection elements and positional data that     */
/*                          share common attributes                          */
/*      primitive       - a non-finite element representation of a           */
/*                          specialized component                            */
/*      eroded element  - an element that is removed from the result space   */
/*                          at a specific simulation                         */

/*      result          - a collection of subcases that share common         */
/*                          analysis properties                              */
/*      subcase         - (aka loadcase) grouping of simulations with the    */
/*                          same physical attributes                         */
/*      simulation      - a snapshot in time of a finite element system      */
/*      datatype        - a specific type of result data (stress, strain,    */
/*                          velocity) computed by a FE solver                */
/*      data set        - data of a specific data type                       */
/*      animation group - listing of datatypes required to visualize         */
/*                          animation of result data                         */

/*      count           - number of items in block                           */
/*      id              - id of model, must be greater than 0 (zero)         */
/*      pool            - a collection of objects whose id values are        */
/*                          unique within the group                          */
/*      tabular format  - entire pool specified in one block, item order     */
/*                          is always maintained                             */
/*      adaptive        - a node pool that grows atspecific simulations,     */
/*                          these position pools must be tabular             */
/*      initial transform - orientation data that is applied to model        */
/*                          display and all simulations                      */
/*      model transform   - orientation data that is applied to the model    */
/*                          or a specified simulation                        */
/*      x, y, z         - orientation translation vector                     */
/*      e1, e2, e3      - orientation rotation vector in radians             */
/*      sim_idx         - index value of specific simulation to which data   */
/*                          applies: H3D_MODEL_SIM_IDX,                      */
/*                                   H3D_GENERIC_RESULT_SIM_IDX              */
/*                                   or other positive value                 */

/*      complex data is to be in magnitude/phase form                        */
/*          (i.e. Complex Vector data (MagX, MagY, MagZ, PhX, PhY, PhZ)      */

#include "h3dpublic_defs.h"

#if defined(_WIN32)
#   define H3D_Export __declspec( dllexport )
#else
#   define H3D_Export
#endif

#if defined (__cplusplus)
extern "C"
{
#endif

/* The Basics */
H3D_Export uint32_t Hyper3DExportLibraryVersion(uint32_t* majorVersion, uint32_t* minorVersion);

H3D_Export H3DFileInfo* Hyper3DExportOpen(const char* filename, H3D_FileMode mode, 
                    H3DMessageFunctionType mFunc, H3DErrorFunctionType eFunc);
                    
H3D_Export bool Hyper3DCompressionLevel(H3DFileInfo* h3d_file, unsigned int level);

H3D_Export H3DFileInfo* Hyper3DExportVersionOpen(const char* filename, H3D_FileMode mode, 
                    uint32_t majorVersion, uint32_t minorVersion, 
                    H3DMessageFunctionType mFunc, H3DErrorFunctionType eFunc);
H3D_Export bool Hyper3DWriteTOC(H3DFileInfo* h3d_file);
H3D_Export bool Hyper3DExportClearError(H3DFileInfo* h3d_file);
H3D_Export bool Hyper3DExportClose(H3DFileInfo* h3d_file);

H3D_Export uint32_t Hyper3DExportFileVersion(H3DFileInfo* h3d_file, uint32_t* majorVersion, uint32_t* minorVersion);
H3D_Export uint32_t Hyper3DExportFileVersion2(const char* filename, uint32_t* majorVersion, uint32_t* minorVersion);

/* String Table Blocks */
H3D_Export bool Hyper3DLookupStringID(H3DFileInfo* h3d_file, const char* const string, H3D_ID* const str_id);
H3D_Export bool Hyper3DLookupString(H3DFileInfo* h3d_file, H3D_ID str_id, const char** string);
H3D_Export bool Hyper3DAddString(H3DFileInfo* h3d_file, const char* const string, H3D_ID* const str_id);

/* alternate messaging APIs */
H3D_Export bool Hyper3DRegisterErrorFunction(H3DFileInfo* h3d_file, H3DErrorFunctionType func);
H3D_Export bool Hyper3DRegisterMessageFunction(H3DFileInfo* h3d_file, H3DMessageFunctionType func);

H3D_Export uint32_t Hyper3DElementStride(H3D_ElementConfig config);
H3D_Export uint32_t Hyper3DDataStride(H3D_DS_FORMAT format);

/* File Information Block - REQUIRED */
/*      creating_appl - name of application creating this file               */
/*      creation_date - date of file creation                                */
/*      solver_name   - name of solver that created data                     */
/*      model_file - file from which model data originated                   */
/*      result_file - file from which result data originated                 */
H3D_Export bool Hyper3DFileInfoBegin(H3DFileInfo* h3d_file, 
                    const char* creating_appl, const char* creation_date, const char* solver_name);
H3D_Export bool Hyper3DFileInfoAddModelFile(H3DFileInfo* h3d_file, const char* model_file);
H3D_Export bool Hyper3DFileInfoAddResultFile(H3DFileInfo* h3d_file, const char* result_file);
H3D_Export bool Hyper3DFileInfoAddComment(H3DFileInfo* h3d_file, const char* comment);
H3D_Export bool Hyper3DFileInfoEnd(H3DFileInfo* h3d_file);


/**************************/
/*  Model Data Functions  */
/**************************/

/* Model Blocks */
/*      name - name of model, may be NULL                                    */
H3D_Export bool Hyper3DModelBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DModelWrite(H3DFileInfo* h3d_file, const char* label, H3D_ID id, bool tabular, H3D_TRIBOOL adaptive);
H3D_Export bool Hyper3DModelEnd(H3DFileInfo* h3d_file);

/* Usage REQUIRED for all for APIs defined below this point */ 
H3D_Export bool Hyper3DSetModelToWrite(H3DFileInfo* h3d_file, H3D_ID id, bool tabular);

/* Assemblies Blocks */
/* * all pool names are specified per block *                                */
/*   AssemblyWrite must be called before corresponding Transform APIs        */
/*                                                                           */
/*      parent_id - id of another assembly                                   */
/*      parent_poolname_id may be H3D_NULL_ID if parent_id == 0  (no parent) */
H3D_Export bool Hyper3DAssemblyBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id, H3D_ID parent_poolname_id);
H3D_Export bool Hyper3DAssemblyWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, H3D_ID parent_id);
H3D_Export bool Hyper3DAssemblyWrite2(H3DFileInfo* h3d_file, uint32_t count,
                    const char** labels, H3D_ID* ids, H3D_ID* parent_ids);
H3D_Export bool Hyper3DAssemblySetInitialTransform(H3DFileInfo* h3d_file, H3D_ID id, 
                    float x, float y, float z, float e1, float e2, float e3);
H3D_Export bool Hyper3DAssemblySetModelTransform(H3DFileInfo* h3d_file, H3D_ID id, 
                    float x, float y, float z, float e1, float e2, float e3);
H3D_Export bool Hyper3DAssemblyEnd(H3DFileInfo* h3d_file);

/* Components Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      parent_id - id of an assembly                                        */
/*      parent_poolname_id may be H3D_NULL_ID if parent_id == 0  (no parent) */
H3D_Export bool Hyper3DComponentBegin(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID poolname_id, H3D_ID parent_poolname_id);
H3D_Export bool Hyper3DComponentWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, H3D_ID node_poolname_id, H3D_ID parent_id);
H3D_Export bool Hyper3DComponentWrite2(H3DFileInfo* h3d_file, uint32_t count, 
                    const char** labels, H3D_ID* ids, H3D_ID* node_poolname_ids, H3D_ID* parent_id);
H3D_Export bool Hyper3DComponentSetInitialTransform(H3DFileInfo* h3d_file, H3D_ID id,
                    float x, float y, float z, float e1, float e2, float e3);
H3D_Export bool Hyper3DComponentSetModelTransform(H3DFileInfo* h3d_file, H3D_ID id, 
                    float x, float y, float z, float e1, float e2, float e3);
H3D_Export bool Hyper3DComponentEnd(H3DFileInfo* h3d_file);

/* Primitives Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      name may be NULL, default is supplied by reader                      */
/*      parent_id - id of an assembly                                        */
/*      parent_poolname_id may be H3D_NULL_ID if parent_id == 0  (no parent) */
H3D_Export bool Hyper3DPrimitiveBegin(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID poolname_id, H3D_ID parent_poolname_id);
H3D_Export bool Hyper3DBoxWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, float xsize, float ysize, float zsize, 
                    bool centered, float x, float y, float z, 
                    float e1, float e2, float e3, H3D_ID parent_id);
H3D_Export bool Hyper3DMarkerWrite(H3DFileInfo* h3d_file, const char* label,
                    H3D_ID id, float x, float y, float z, 
                    float e1, float e2, float e3, H3D_ID parent_id);
H3D_Export bool Hyper3DCylinderWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, int angle, int segments_in_yz, 
                    float sweep_angle, H3D_PRIMITIVE_CAPS cap, float semi_x, 
                    float semi_y1, float semi_y2, float semi_z1, float semi_z2,
                    bool centered, float x, float y, float z, 
                    float e1, float e2, float e3, H3D_ID parent_id);
H3D_Export bool Hyper3DEllipsoidWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, int angle, int segments_in_z, int segments_in_xy,
                    float semi_x, float semi_y, float semi_z,
                    float x, float y, float z, 
                    float e1, float e2, float e3, H3D_ID parent_id);
H3D_Export bool Hyper3DLinesWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, uint32_t* elem_conn, uint32_t conn_length, float* nodes, 
                    uint32_t num_nodes, float x, float y, float z, 
                    float e1, float e2, float e3, H3D_ID parent_id);
H3D_Export bool Hyper3DMeshWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, uint32_t* conn, uint32_t conn_length, float* nodes, uint32_t num_nodes,
                    float x, float y, float z, float e1, float e2, float e3, H3D_ID parent_id);
H3D_Export bool Hyper3DOutlineWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, uint32_t num_markers, H3D_ID* marker_ids, 
                    H3D_ID* marker_poolname_ids, H3D_ID parent_id);
H3D_Export bool Hyper3DSpringWrite(H3DFileInfo* h3d_file, const char* label, H3D_ID id,
                    H3D_ID marker1_id, H3D_ID marker1_poolname_id,
                    H3D_ID marker2_id, H3D_ID marker2_poolname_id,
                    float offset_marker1_damper, float offset_marker2_damper,
                    float height_damper1, float height_damper2,
                    float diameter_spring, float diameter_damper1, float diameter_damper2,
                    int helix_coils, int helix_segs, H3D_ID parent_id);
H3D_Export bool Hyper3DPrimitiveSetInitialTransform(H3DFileInfo* h3d_file, 
                    H3D_ID id, float x, float y, float z, float e1, float e2, float e3);
H3D_Export bool Hyper3DPrimitiveEnd(H3DFileInfo* h3d_file);

/* System Blocks */
/* at this time H3D supports only one system pool                            */
/*      and systems may not be nested (or parented)                          */
H3D_Export bool Hyper3DSystemBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DSystemWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_SYSTEM_TYPE type, float* global_origin, float* global_axis);
H3D_Export bool Hyper3DSystemEnd(H3DFileInfo* h3d_file);

/* systems that follow the node definitions during animation                 */
H3D_Export bool Hyper3DSystem2Begin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID pool_id);
H3D_Export bool Hyper3DSystem2Write(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_SYSTEM_TYPE type, bool fixed, H3D_SystemDefMethod def, 
                    H3D_ID npool_id, H3D_ID node_id1, H3D_ID node_id2, H3D_ID node_id3);
H3D_Export bool Hyper3DSystem2End(H3DFileInfo* h3d_file);


/* Node blocks */
/* * all pool names are specified per block                                 */
/*   PositionR API is used only when suppling time indexed positions         */
/*      as opposed to time indexed nodal displacement results                */
/*   PositionR API may only be used when defining a tabular model            */
/*   PositionR is specified per block                                        */
H3D_Export bool Hyper3DPositionBegin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID poolname_id);
H3D_Export bool Hyper3DPositionWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    float* coords, H3D_ID refsys_id, H3D_ID analysis_id);
H3D_Export bool Hyper3DPositionWrite2(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, float* coords, H3D_ID* refsys_ids, H3D_ID* analysis_ids);
H3D_Export bool Hyper3DPositionEnd(H3DFileInfo* h3d_file);

H3D_Export bool Hyper3DPositionRBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id, H3D_SIM_IDX idx, H3D_ID subcase_id); 
H3D_Export bool Hyper3DPositionRWrite(H3DFileInfo* h3d_file, float* coords);
H3D_Export bool Hyper3DPositionRWrite2(H3DFileInfo* h3d_context, uint32_t count, float* coords);
H3D_Export bool Hyper3DPositionREnd(H3DFileInfo* h3d_file);

/* Position Pool blocks */
/*  this block is used to specify a position data pool in tabular models     */
/*  this block also expresses the id order that a result data blocks for     */
/*      the named pool will have                                             */
H3D_Export bool Hyper3DPositionPoolBegin(H3DFileInfo* h3d_file, H3D_ID poolname_id);
H3D_Export bool Hyper3DPositionPoolWrite(H3DFileInfo* h3d_file, 
                    uint32_t num_ids, H3D_ID* ids);
H3D_Export bool Hyper3DPositionPoolEnd(H3DFileInfo* h3d_file);

/* Element Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      parent_id - id of a component                                        */
/*      connectivity will be node ids,                                       */
/*          except in tabluar models when it will be node pool indexes       */
/*                                                                           */
/*      any elem_pname begining with "HW_LOD_" is an pool of DAB data        */
/*          from Hyperworks and is not considered part of the actual         */
/*          FE Model definition                                              */
H3D_Export bool Hyper3DElementBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id, H3D_ElementConfig config, H3D_ID type_id,
                    H3D_ID parent_id, H3D_ID parent_poolname_id, H3D_ID node_poolname_id);
H3D_Export bool Hyper3DElementRBegin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID poolname_id, 
                    H3D_ElementConfig config, H3D_ID type_id, H3D_ID parent_id, 
                    H3D_ID parent_poolname_id, H3D_ID node_poolname_id, 
                    H3D_SIM_IDX idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DElementWrite(H3DFileInfo* h3d_file, H3D_ID id, H3D_ID* connectivity);
H3D_Export bool Hyper3DElementWrite2(H3DFileInfo* h3d_file, uint32_t count, H3D_ID* id, H3D_ID* connectivity);
H3D_Export bool Hyper3DElementEnd(H3DFileInfo* h3d_file);

/* Elements (second form) */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      parent_id - id of a component                                        */
/*      connectivity will be node ids,                                       */
/*          except in tabluar models when it will be node pool indexes       */
/*                                                                           */
/*      inode, idof, icoeff, num_inodes are the independant nodes of an      */
/*          element, while dnode, ddof, dcoeff, num_dnodes are the           */
/*          dependant nodes                                                  */
H3D_Export bool Hyper3DElement2Begin(H3DFileInfo* h3d_file,  uint32_t count,
                    H3D_ID poolname_id, H3D_ElementConfig config, H3D_ID type_id,
                    H3D_ID parent_id, H3D_ID parent_poolname_id, H3D_ID node_poolname_id);
H3D_Export bool Hyper3DElement2RBegin(H3DFileInfo* h3d_file,  
                    uint32_t count, H3D_ID poolname_id, 
                    H3D_ElementConfig config, H3D_ID type_id, H3D_ID parent_id, 
                    H3D_ID parent_poolname_id, H3D_ID node_poolname_id, 
                    H3D_SIM_IDX idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DElement2Write(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID* inode, int* idof, double* icoeff, uint32_t num_inodes,
                    H3D_ID* dnode, int* ddof, double* dcoeff, uint32_t num_dnodes);
H3D_Export bool Hyper3DElement2End(H3DFileInfo* h3d_file);


/* Elements by Pool blocks */
/*  this block is used to specify a element data pool in tabular models      */
/*  this block also expresses the id order that a result data blocks for     */
/*      the named pool will have                                             */
/*  as this block is used for tabluar model only, the element connectivity   */
/*      is to be specified by the position pool index                        */
H3D_Export bool Hyper3DElementPoolBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id, H3D_ID parent_poolname_id, H3D_ID node_poolname_id);
H3D_Export bool Hyper3DElementPoolRBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id, H3D_ID parent_poolname_id, 
                    H3D_ID node_poolname_id, H3D_SIM_IDX idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DElementPoolWrite(H3DFileInfo* h3d_file, 
                    H3D_ElementConfig config, H3D_ID id, H3D_ID parent_id, H3D_ID* connectivity);
H3D_Export bool Hyper3DElementPoolEnd(H3DFileInfo* h3d_file);


/* Property blocks*/
H3D_Export bool Hyper3DPropShellBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DPropShellWrite(H3DFileInfo* h3d_file, H3D_ID label_id, H3D_ID id, 
                    H3D_ID mid1, float thick1, H3D_ID mid2, float bend, H3D_ID mid3, 
                    float thick2, float mass, float z1dist, float z2dist, H3D_ID mid4);
H3D_Export bool Hyper3DPropShellAddElements(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID elem_pool_id, uint32_t num_elems, H3D_ID* elem_ids);
H3D_Export bool Hyper3DPropShellAddElementSets(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID set_pool_id, uint32_t num_sets, H3D_ID* set_ids);
H3D_Export bool Hyper3DPropShellAddComponent(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID comp_pool_id, uint32_t num_comps, H3D_ID* comp_ids);
H3D_Export bool Hyper3DPropShellEnd(H3DFileInfo* h3d_file);

H3D_Export bool Hyper3DPropSolidBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DPropSolidWrite(H3DFileInfo* h3d_file, H3D_ID label_id, H3D_ID id, 
                    H3D_ID mid, H3D_ID cordm, H3D_PROP_ISOP isop, H3D_PROP_FCTN fctn, float ds);
H3D_Export bool Hyper3DPropSolidAddElements(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID elem_pool_id, uint32_t num_elems, H3D_ID* elem_ids);
H3D_Export bool Hyper3DPropSolidAddElementSets(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID set_pool_id, uint32_t num_sets, H3D_ID* set_ids);
H3D_Export bool Hyper3DPropSolidlAddComponent(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID comp_pool_id, uint32_t num_comps, H3D_ID* comp_ids);
H3D_Export bool Hyper3DPropSolidEnd(H3DFileInfo* h3d_file);


/* Material blocks*/
H3D_Export bool Hyper3DMaterial1Begin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DMaterial1Write(H3DFileInfo* h3d_file, H3D_ID label_id, H3D_ID id, float e, 
                    float g, float nu, float rho, float a, float tref, float ge,  
                    float st, float sc, float ss, H3D_ID sys_id);
H3D_Export bool Hyper3DMaterial1End(H3DFileInfo* h3d_file);

H3D_Export bool Hyper3DMaterial2Begin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DMaterial2Write(H3DFileInfo* h3d_file, H3D_ID label_id, H3D_ID id, 
                    float* gij, float rho, float* aj, float tref, float ge, float st, 
                    float sc, float ss);
H3D_Export bool Hyper3DMaterial2End(H3DFileInfo* h3d_file);

H3D_Export bool Hyper3DMaterial8Begin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DMaterial8Write(H3DFileInfo* h3d_file, H3D_ID label_id, H3D_ID id, 
                    float e1, float e2, float nu, float g12, float g1z, float g2z, float rho, 
                    float a1, float a2, float tref, float xt, float xc, float yt, 
                    float yc, float s, float ge, float f, float strn);
H3D_Export bool Hyper3DMaterial8End(H3DFileInfo* h3d_file);

H3D_Export bool Hyper3DMaterial9Begin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DMaterial9Write(H3DFileInfo* h3d_file, H3D_ID label_id, H3D_ID id, 
                    float* gij, float rho, float* ai, float tref, float ge);
H3D_Export bool Hyper3DMaterial9End(H3DFileInfo* h3d_file);


/* Entity Set Blocks */
H3D_Export bool Hyper3DEntitySetBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id);
H3D_Export bool Hyper3DEntitySetWrite(H3DFileInfo* h3d_file, const char* label,
                    H3D_ID id, H3D_ID_POOL_TYPE ent_type);
H3D_Export bool Hyper3DEntitySetAdd(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID_POOL_TYPE ent_type, H3D_ID ent_poolname_id, 
                    H3D_ID* ids, uint32_t num_ids);
H3D_Export bool Hyper3DEntitySetAddRange(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID_POOL_TYPE ent_type, H3D_ID ent_poolname_id, 
                    H3D_ID first_id, H3D_ID last_id);
H3D_Export bool Hyper3DEntitySetRemove(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID_POOL_TYPE ent_type, H3D_ID ent_poolname_id, 
                    H3D_ID* ids, uint32_t num_ids);
H3D_Export bool Hyper3DEntitySetRemoveRange(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID_POOL_TYPE ent_type, H3D_ID ent_poolname_id, 
                    H3D_ID first_id, H3D_ID last_id);
H3D_Export bool Hyper3DEntitySetEnd(H3DFileInfo* h3d_file);


/*************************************/
/*  Model Attribute Export Functions */
/*************************************/

/* Model Attributes Blocks */
H3D_Export bool Hyper3DModelAttribBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DModelAttribWrite(H3DFileInfo* h3d_file, H3D_ID id,
                    H3D_MODEL_ATTRIBS flags, uint8_t* meta, uint32_t meta_size);
H3D_Export bool Hyper3DModelAttribColor(H3DFileInfo* h3d_file, H3D_ID id, uint8_t color[3]);
H3D_Export bool Hyper3DModelAttribEnd(H3DFileInfo* h3d_file);

/* Assembly Attributes Blocks */
/* * all pool names are specified per block *                                */
H3D_Export bool Hyper3DAssemblyAttribBegin(H3DFileInfo* h3d_file, 
                    uint32_t count, H3D_ID poolname_id);
H3D_Export bool Hyper3DAssemblyAttribWrite(H3DFileInfo* h3d_file, H3D_ID id, H3D_ASSEMBLY_ATTRIBS flags);
H3D_Export bool Hyper3DAssemblyAttribColor(H3DFileInfo* h3d_file, H3D_ID id, uint8_t color[3]);
H3D_Export bool Hyper3DAssemblyAttribEnd(H3DFileInfo* h3d_file);

/* Component Attributes Blocks */
/* * all pool names are specified per block *                                */
/*   AttribWrite must be called before any other corresponding APIs          */
/*                                                                           */
/*      color - RGB values                                                   */
H3D_Export bool Hyper3DComponentAttribBegin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID poolname_id);
H3D_Export bool Hyper3DComponentAttribWrite(H3DFileInfo* h3d_file, H3D_ID id, H3D_COMPONENT_ATTRIBS flags);
H3D_Export bool Hyper3DComponentAttribColor(H3DFileInfo* h3d_file, H3D_ID id, uint8_t color[3]);
H3D_Export bool Hyper3DComponentAttribGraphicMaterial(H3DFileInfo* h3d_file, H3D_ID id, const char* gmat_label);
H3D_Export bool Hyper3DComponentAttribFeatureAngle(H3DFileInfo* h3d_file, H3D_ID id, float featureAngle);
H3D_Export bool Hyper3DComponentAttribEnd(H3DFileInfo* h3d_file);

/* System Attributes Blocks */
/*      system_pname - this agruement should be NULL and will be ignored     */
/*      color - RGB values                                                   */
H3D_Export bool Hyper3DSystemAttribBegin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID poolname_id);
H3D_Export bool Hyper3DSystemAttribWrite(H3DFileInfo* h3d_file, H3D_ID id, H3D_SYSTEM_ATTRIBS flags, uint8_t color[3]);
H3D_Export bool Hyper3DSystemAttribEnd(H3DFileInfo* h3d_file);

/* Entity Set Attributes Blocks */
/*      color - RGB values                                                   */
/*      draw_size - use default size, set to 0                               */
H3D_Export bool Hyper3DEntitySetAttribBegin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID poolname_id);
H3D_Export bool Hyper3DEntitySetAttribWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ENTITY_SET_ATTRIBS flags, uint32_t draw_size, uint8_t color[3]);
H3D_Export bool Hyper3DEntitySetAttribEnd(H3DFileInfo* h3d_file);

/* Eroded Data Blocks */
/* * all pool names are specified per block *                                */
H3D_Export bool Hyper3DErodeBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID element_poolname_id, H3D_SIM_IDX idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DErodeElement(H3DFileInfo* h3d_file, H3D_ID id);
H3D_Export bool Hyper3DErodeEnd(H3DFileInfo* h3d_file);

/* Graphoic Dsiplay Materials */
H3D_Export bool Hyper3DGraphicMaterialsBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DGraphicMaterialsWrite(H3DFileInfo* h3d_file, const char* label, 
                    float alpha, const float* ambient, const float* diffuse, 
                    const float* emission, float metallic, float roughness, 
                    float shininess, const float* specular);
H3D_Export bool Hyper3DGraphicMaterialsEnd(H3DFileInfo* h3d_file);

/* Mask Blocks */
/* * all pool names are specified per block *                                */
/* * only one entity type allowed per block *                                */
/*  HyperView's default is to mask all markers and entity sets when imported,*/
/*      thus markers and entity sets are only unmasked (or made visible)     */
H3D_Export bool Hyper3DMaskBegin(H3DFileInfo* h3d_file, uint32_t count, H3D_ID poolname_id, H3D_ID view_id);
H3D_Export bool Hyper3DMaskComponent(H3DFileInfo* h3d_file, H3D_ID id);
H3D_Export bool Hyper3DMaskSystem(H3DFileInfo* h3d_file, H3D_ID id);
H3D_Export bool Hyper3DUnmaskMarker(H3DFileInfo* h3d_file, H3D_ID id);
H3D_Export bool Hyper3DUnmaskEntitySet(H3DFileInfo* h3d_file, H3D_ID id);
H3D_Export bool Hyper3DMaskElementRange(H3DFileInfo* h3d_file, H3D_ID first_id, H3D_ID last_id);
H3D_Export bool Hyper3DMaskElement(H3DFileInfo* h3d_file, H3D_ID id);
H3D_Export bool Hyper3DMaskEnd(H3DFileInfo* h3d_file);

/* Text blocks */
H3D_Export bool Hyper3DFATXMLWrite(H3DFileInfo* h3d_file, uint32_t text_size, const char* text_buffer);
H3D_Export bool Hyper3DTextWrite(H3DFileInfo* h3d_file, const char* text_tag, 
                    H3D_SIM_IDX idx, H3D_ID subcase_id, 
                    uint32_t text_size, const char* text_buffer);

/* View Blocks */
H3D_Export bool Hyper3DViewBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DViewWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_VIEW_TYPE type, float* view_matrix);
H3D_Export bool Hyper3DViewOrthoWrite(H3DFileInfo* h3d_file, const char* label,
                    float xmin, float xmax, float ymin, float ymax, 
                    float zmin, float zmax);
H3D_Export bool Hyper3DViewPerspectiveWrite(H3DFileInfo* h3d_file, const char* label,
                    float xmin, float xmax, float ymin, float ymax, 
                    float zmin, float zmax, float fov, float z_offset);
H3D_Export bool Hyper3DViewLensWrite(H3DFileInfo* h3d_file, const char* label,
                    float angle, float focal_len, float sensor_ht, 
                    float x_orient, float y_orient, float z_orient,
                    float x_offset, float y_offset, float z_offset,
                    float inp_sensor_ht, float inp_x_focal_pt, 
                    float inp_y_focal_pt, H3D_ID img_plane_ID, bool multiPt, 
                    float top_crop, float bot_crop, float left_crop, 
                    float right_crop);
H3D_Export bool Hyper3DViewEnd(H3DFileInfo* h3d_file);


/**********************************/
/*  Result Data Export Functions  */
/**********************************/

/* Result Blocks */
/*  the purpose of this block is to establish defaults for interpreting      */
/*      the result data contained in this file                               */
/*                                                                           */
/*      count   - please use only 1 at this time                             */
/*      name    - may be NULL                                                */
H3D_Export bool Hyper3DResultBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DResultWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_DS_SHELL_METHOD method, uint32_t num_systems);
H3D_Export bool Hyper3DResultAddSystem(H3DFileInfo* h3d_file, 
                    H3D_ID poolname_id, int system_id, 
                    H3D_ID_POOL_TYPE sysType);
H3D_Export bool Hyper3DResultEnd(H3DFileInfo* h3d_file);

/* Subcase (Loadcase) Blocks */
/*  animation groups must be defined in order for the reader to determine    */
/*      which vector data types can be animated                              */
/*  HyperView's default is to animate none                                   */
H3D_Export bool Hyper3DSimSubcaseBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DSimSubcaseWrite(H3DFileInfo* h3d_file, const char* label,
                    H3D_ID id, H3D_ANALYSIS_TYPE atype,  
                    uint32_t num_datatypes, H3D_ID* datatype_ids, 
                    H3D_NODAL_DATA_TYPE anim_type);
H3D_Export bool Hyper3DSimSubcaseAnimationGroups(H3DFileInfo* h3d_file, 
                    H3D_ID id, 
                    uint32_t num_groups, H3D_ID* grp_datatype_ids, 
                    uint32_t* num_dts_per_grp, H3D_ID* datatype_ids);
H3D_Export bool Hyper3DSimSubcaseEnd(H3DFileInfo* h3d_file);

/* Derived Subcase Blocks */
H3D_Export bool Hyper3DDerivedSubcaseBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DDerivedSubcaseWrite(H3DFileInfo* h3d_file, const char* label,
                    H3D_ID id, H3D_SUBCASE_TYPE type, uint32_t num_sims);
H3D_Export bool Hyper3DDerivedSubcaseSimWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID ref_subcase_id, H3D_SIM_IDX ref_sim_idx, float scale);
H3D_Export bool Hyper3DDerivedSubcaseEnd(H3DFileInfo* h3d_file);

/* Simulation Blocks */
/* * all time steps in this block are per subcase                            */
H3D_Export bool Hyper3DSimulationBegin(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID subcase_id);
H3D_Export bool Hyper3DSimulationWrite(H3DFileInfo* h3d_file, H3D_SIM_IDX idx, 
                    const char* label, float syncValue); 
H3D_Export bool Hyper3DSimulationTabularWrite(H3DFileInfo* h3d_file, 
                    H3D_SIM_IDX idx, const char* label, float syncValue, 
                    bool adaptive, bool eroded); 
H3D_Export bool Hyper3DSimulationEnd(H3DFileInfo* h3d_file);

/* Datatype Blocks */
/*   DatatypeWrite must be called before corresponding DatatypePool API      */
/*                                                                           */
/*  an example:                                                              */
/*      2 calculations of Von Mises Stress data to be available, Z1 & Z2     */
/*      create a data type labeled "Stress:Von Mises" with 2 layers          */
/*      the ':' is defined as H3D_DT_DELIMITER                               */
/*      there can be multiple Stress:.... data types, they will be collated  */
H3D_Export bool Hyper3DDatatypeBegin(H3DFileInfo* h3d_file, uint32_t count);
H3D_Export bool Hyper3DDatatypeWrite(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID dt_id, H3D_DS_FORMAT format, H3D_DS_TYPE type, 
                    H3D_NF_FORMAT nu_form, uint32_t num_pools);
H3D_Export bool Hyper3DDatatypeDescriptionWrite(H3DFileInfo* h3d_file, 
                    H3D_ID dt_id, const char* description);
H3D_Export bool Hyper3DDatatypePools(H3DFileInfo* h3d_file, H3D_ID dt_id, 
                    H3D_ID poolname_id, uint32_t num_layers, 
                    H3D_ID* layername_ids, bool corners,
                    H3D_TENSOR_TYPE tensor_type, float poisson);
H3D_Export bool Hyper3DDatatypeEnd(H3DFileInfo* h3d_file);

/* Dataset Blocks */
/* * all pool names are specified per block *                                */
/*  WriteParent is used when specifying nodal data and the node is on a      */
/*      component boundary and shared with another component                 */
/*  WriteWithSystem is used when this data is associated to a system         */
/*      different from that described in the Result Block default for the    */
/*      specified pool                                                       */
/*  Write is used when all defaults are to be applied                        */
/*                                                                           */
/*      num_corners - element centroidal and corner data must be specified   */
/*                      in separate blocks                                   */
/*      num_modes   - should be zero, this field only used by MotionSolve    */
/*      dt_id       - data type id associated with this set                  */
/*      layer_idx   - specific layer index of dt_id of this set              */
/*                      note: H3D_DS_NO_LAYER is specified if there is no    */
/*                            layer associated of the dt_id to this pool     */
/*      data_poolname_id  - poolname id where entity IDs are defined         */
/*      complex data is to be in magnitude/phase form                        */
/*          (i.e. Complex Vector data (MagX, MagY, MagZ, PhX, PhY, PhZ)      */
H3D_Export bool Hyper3DDatasetBegin(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX idx, H3D_ID subcase_id, H3D_DS_TYPE type, 
                    H3D_DS_FORMAT format, H3D_NF_FORMAT nu_form, 
                    uint32_t num_corners, uint32_t num_modes, 
                    H3D_ID dt_id, int layer_idx, H3D_ID data_poolname_id);
H3D_Export bool Hyper3DDatasetWriteParent(H3DFileInfo* h3d_file, H3D_ID comp_id, 
                    H3D_ID component_poolname_id);

H3D_Export bool Hyper3DDatasetWrite(H3DFileInfo* h3d_file, H3D_ID id,
                    const float* data);
H3D_Export bool Hyper3DDatasetWrite2(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, const float* data);
H3D_Export bool Hyper3DDatasetWriteWithSystem(H3DFileInfo* h3d_file, H3D_ID id, 
                    const float* data, H3D_ID sys_id, H3D_ANALYSIS_SYSTEM system_flag);
H3D_Export bool Hyper3DDatasetWriteWithSystem2(H3DFileInfo* h3d_file, uint32_t count, H3D_ID* ids, 
                    const float* data, H3D_ID* sys_id, H3D_ANALYSIS_SYSTEM* system_flag);

H3D_Export bool Hyper3DDatasetDoubleWrite(H3DFileInfo* h3d_file, H3D_ID id,
                    const double* data);
H3D_Export bool Hyper3DDatasetDoubleWrite2(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID id, const double* data);
H3D_Export bool Hyper3DDatasetDoubleWriteWithSystem(H3DFileInfo* h3d_file, H3D_ID id, 
                    const double* data, H3D_ID sys_id, H3D_ANALYSIS_SYSTEM system_flag);
H3D_Export bool Hyper3DDatasetDoubleWriteWithSystem2(H3DFileInfo* h3d_file, uint32_t count, H3D_ID* ids, 
                    const double* data, H3D_ID* sys_id, H3D_ANALYSIS_SYSTEM* system_flag);

H3D_Export bool Hyper3DDatasetEnd(H3DFileInfo* h3d_file);


/* Flexbody Datasets */
H3D_Export bool Hyper3DFlexDatasetBegin(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_FLEX_TYPE type,
                    uint32_t count, H3D_ID data_poolname_id, 
                    H3D_SIM_IDX idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DFlexDatasetWithIdsBegin(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_FLEX_TYPE type, 
                    uint32_t count, H3D_ID* data_ids, H3D_ID data_poolname_id, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DFlexDatasetWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_FLEX_TYPE type, void* data);
H3D_Export bool Hyper3DFlexDatasetEnd(H3DFileInfo* h3d_file);

/* Flexbody Mode Blocked Datasets */
H3D_Export bool Hyper3DFlexBlockDatasetCompressionLevel(H3DFileInfo* h3d_file, uint32_t level);
H3D_Export bool Hyper3DFlexBlockDatasetBegin(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_FLEX_TYPE type, uint32_t num_blocks, 
                    H3D_ID* data_ids, H3D_ID data_poolname_id, uint32_t id_count, 
                    uint32_t data_count, H3D_SIM_IDX sim_idx, H3D_ID subcase_id);
H3D_Export bool Hyper3DFlexBlockDatasetWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_FLEX_TYPE type, uint32_t block_idx, void* data);
H3D_Export bool Hyper3DFlexBlockDatasetEnd(H3DFileInfo* h3d_file);

/* Matrix Datasets */
H3D_Export bool Hyper3DMatrixBegin(H3DFileInfo* h3d_file, const char* label, 
                    H3D_ID id, H3D_ID poolname_id, H3D_MTX_TYPE type, 
                    H3D_MTX_FORMAT mxt_type, H3D_MTX_VALUE_FORMAT val_type,
                    uint32_t num_columns, uint32_t num_rows,
                    uint64_t nz_count, 
                    H3D_ID parent_id, H3D_ID parent_poolname_id,
                    H3D_SIM_IDX idx, H3D_ID subcase_id);

/* all matrix labels must be written before            */
/*      ANY of the matrix data write commands are used */
H3D_Export bool Hyper3DMatrixLabelWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID column_id, H3D_ID row_id, H3D_ID label_id, 
                    H3D_ID entity_id, H3D_ID ent_poolname_id);

H3D_Export bool Hyper3DDenseMtxWrite(H3DFileInfo* h3d_file, H3D_ID id,
                    uint32_t start_column_id, 
                    uint32_t num_cols, void* data);

/* For a sparse m * n matrix, M, we need to store three one-dimensional        */
/* arrays. Let NNZ denote the number of nonzero entries of M.                */
/* The first array is A, which is of length NNZ, and holds all nonzero       */
/* entries of M in top-to-bottom left-to-right order.                        */
/* The second array is JA, which is of length n + 1. JA(i) contains the      */
/* index in A of the first nonzero element of column i. Column i of the      */
/* original matrix extends from A(JA(i)) to A(JA(i+1)-1).                    */
/* The third array, IA, contains the row index of each element of A,         */
/* so it also is of length NNZ.                                              */
H3D_Export bool Hyper3DSparseMtxJaWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    uint32_t* data);
H3D_Export bool Hyper3DSparseMtxIaAaWrite(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID start_column_id, uint32_t num_columns, 
                    uint32_t* dataJa, uint32_t* dataIa, void* dataAa);
H3D_Export bool Hyper3DMatrixEnd(H3DFileInfo* h3d_file);


#if defined (__cplusplus)
}
#endif
