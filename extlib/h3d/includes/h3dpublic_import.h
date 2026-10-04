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

/* ///////////////////////////////////////////////////////////////////////// */
/*  H3D callback functions                                                   */
/*    CreateH3DReaderFunctionTable(H3DReaderFunctionTable*);  must be called */
/* ///////////////////////////////////////////////////////////////////////// */

/* The Basics */
H3D_Export uint32_t Hyper3DImportLibraryVersion(uint32_t* majorVersion, uint32_t* minorVersion);

/* quick open for reader verify functions */
H3D_Export H3DFileInfo* Hyper3DImportVerifyOpen(const char* filename, H3DMessageFunctionType mFunc, H3DErrorFunctionType eFunc);  
H3D_Export bool         Hyper3DImportOpenVerified(H3DFileInfo* h3d_file);
/* full open for reading data */   
H3D_Export H3DFileInfo* Hyper3DImportOpen(const char* filename, H3DMessageFunctionType mFunc, H3DErrorFunctionType eFunc);
H3D_Export bool         Hyper3DImportClose(H3DFileInfo* h3d_file);

H3D_Export uint32_t Hyper3DImportFileVersion(H3DFileInfo* h3d_file, uint32_t* majorVersion, uint32_t* minorVersion);
H3D_Export uint32_t Hyper3DImportFileVersion2(const char* filename, uint32_t* majorVersion, uint32_t* minorVersion);

H3D_Export uint32_t Hyper3DElementStride(H3D_ElementConfig config);
H3D_Export uint32_t Hyper3DDataStride(H3D_DS_FORMAT format);


/* String Table Blocks */
/*  The string blocks need not be read but this API is provided for          */
/*  completeness. The Lookup API functions have been provided for easy       */
/*  conversion of Sting IDs to actual string values.                         */
H3D_Export bool Hyper3DCountStrings(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DLookupStringID(H3DFileInfo* h3d_file, const char* const string, H3D_ID* const str_id);
H3D_Export bool Hyper3DLookupString(H3DFileInfo* h3d_file, H3D_ID str_id, const char** string);

H3D_Export bool Hyper3DReadStringTable(H3DFileInfo* h3d_file);
           bool H3DCreateStrings(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddString(H3DFileInfo* h3d_file, H3D_ID id, const char* string);

/* Byte-swapping utilities */
H3D_Export bool Hyper3DSwap(H3DFileInfo* h3d_file, void* data, uint32_t sizeInBytes);
H3D_Export bool Hyper3DSwapArray(H3DFileInfo* h3d_file, void* data, uint32_t sizeInBytes, uint32_t count);

/* alternate messaging APIs */
H3D_Export bool  Hyper3DRegisterErrorFunction(H3DFileInfo* h3d_file, H3DErrorFunctionType func);
H3D_Export bool  Hyper3DRegisterMessageFunction(H3DFileInfo* h3d_file, H3DMessageFunctionType func);

/* File Information Block - REQUIRED */
/*      creating_appl - name of application creating this file               */
/*      creation_date - date of file creation                                */
/*      model_file - file from which model data originated                   */
/*      result_file - file from which result data originated                 */
H3D_Export bool Hyper3DReadFileInfo(H3DFileInfo* h3d_file);
           bool H3DFileInformation(H3DFileInfo* h3d_file, 
                    const char* appl_name, const char* creation_date, const char* solver_name, 
                    const char** model_filenames, uint32_t num_models, 
                    const char** result_filenames, uint32_t num_results,
                    const char** comments, uint32_t num_comments); 

/************************/
/* Model Data Functions */
/************************/

/* Model Blocks */
/*      name - name of model, may be NULL                                    */
/* Model Block Callbacks */
H3D_Export bool Hyper3DReadModels(H3DFileInfo* h3d_file);
           bool H3DCreateModels(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddModel(H3DFileInfo* h3d_file, const char* name, H3D_ID id,
                    H3D_TRIBOOL model_is_tabular, H3D_TRIBOOL model_is_adaptive, 
                    double quantize_error);

H3D_Export bool Hyper3DSetModelToRead(H3DFileInfo* h3d_file, H3D_ID id);


/* Assemblies Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      parent_id - id of another assembly                                   */
/*      parent_pname may be NULL if parent_id == 0 (no parent)               */
/* Assemblies Block Callbacks */
H3D_Export bool Hyper3DReadAssemblies(H3DFileInfo* h3d_file);
           bool H3DCreateAssemblies(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID poolname_id);
           bool H3DAddAssemblyToAssembly(H3DFileInfo* h3d_file, 
                    const char* name, H3D_ID id, H3D_ID poolname_id,
                    float ix, float iy, float iz, float ie1, float ie2, float ie3,
                    float mx, float my, float mz, float me1, float me2, float me3,
                    H3D_ID parent_id, H3D_ID parent_poolname_id);


/* Components Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      parent_id - id of an assembly                                        */
/*      parent_pname may be NULL if parent_id == 0  (no parent)              */
/*                                                                           */
/*      orientation is a float[12] grouping per ID                           */
/*            {ix, iy, iz, ie1, ie2, ie3, mx, my, mz, me1, me2, me3}         */
/* Components Block Callbacks */
H3D_Export bool Hyper3DReadComponents(H3DFileInfo* h3d_file);
           bool H3DCreateComponents(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID poolname_id);
           bool H3DAddComponentToAssembly(H3DFileInfo* h3d_file, 
                    const char* name, H3D_ID id, H3D_ID poolname_id, 
                    float ix, float iy, float iz, float ie1, float ie2, float ie3,
                    float mx, float my, float mz, float me1, float me2, float me3,
                    H3D_ID parent_id, H3D_ID parent_poolname_id,
                    H3D_ID node_poolname_id); 
                
/* Primitives Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      name may be NULL, read must supply default                           */
/*      parent_id - id of an assembly                                        */
/*      parent_pname may be NULL if parent_id == 0 (no parent)               */
/* Primitives Block Callbacks */
H3D_Export bool Hyper3DReadPrimitives(H3DFileInfo* h3d_file);
           bool H3DCreatePrimitives(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID poolname_id);
           bool H3DAddPrimitiveToAssembly(H3DFileInfo* h3d_file, 
                    const char* name, H3D_ID id,  H3D_ID poolname_id, void* param, 
                    float ix, float iy, float iz, float ie1, float ie2, float ie3, 
                    float mx, float my, float mz, float me1, float me2, float me3, 
                    H3D_ID parent_id, H3D_ID parent_poolname_id); 

/* System Blocks */
/* at this time H3D supports only one system pool                            */
/*      and systems may not be nested (or parented)                          */
/* System Block Callbacks */
H3D_Export bool Hyper3DReadSystems(H3DFileInfo* h3d_file);
           bool H3DCreateSystems(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddSystem(H3DFileInfo* h3d_file, H3D_ID id,
                    H3D_SYSTEM_TYPE type, float global_origin[3], 
                    float global_axis[3][3]);

/* Position Blocks */
/* * all pool names are specified per block *                                */
/* Position Block Callbacks */
H3D_Export bool Hyper3DReadPositionsCountR(H3DFileInfo* h3d_file, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, 
                    H3D_ID poolname_id, uint32_t& num_positions);
H3D_Export bool Hyper3DReadPositionsCount2(H3DFileInfo* h3d_file);
H3D_Export bool Hyper3DReadPositionsCountR2(H3DFileInfo* h3d_file, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id);
           bool H3DCountPositionPools2(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id,
                    H3D_ID* poolname_ids, uint32_t* num_positions);

H3D_Export bool Hyper3DReadPositions(H3DFileInfo* h3d_file);
H3D_Export bool Hyper3DReadPositionsR(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID subcase_id);
           bool H3DCreatePositions(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, float** coords, 
                    H3D_ID* ids, H3D_ID poolname_id, 
                    H3D_ID* refsys_ids, H3D_ID* ansys_ids);

/* Position Pool Blocks */
/*  this block is used to specify a position data pool in tabular models     */
/*  this block also expresses the id order that a result data blocks for     */
/*      the named pool will have                                             */
/*  at this time it is recommended that only one pool be stored per block    */
/*      as the ammount of memory required to read this data can be very      */
/*      large per pool                                                       */
/* Position Pool Block Callbacks */
H3D_Export bool Hyper3DReadPositionPools(H3DFileInfo* h3d_file);
           bool H3DCreatePositionPool(H3DFileInfo* h3d_file, uint32_t num_nodes,
                    H3D_ID* ids, H3D_ID poolname_id);



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
/* Element counts */
/*     pool_name may be NULL if searching all pools                          */
H3D_Export bool Hyper3DReadElementCount(H3DFileInfo* h3d_file, 
                    H3D_ID poolname_id);
H3D_Export bool Hyper3DReadElementCountR(H3DFileInfo* h3d_file, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID subcase_id);
           bool H3DCountElementPool(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, 
                    H3D_ID poolname_id, H3D_ElementConfig config,
                    H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
           bool H3DCountElementPoolMixed(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, 
                    H3D_ID poolname_id, H3D_ElementConfig* config,
                    H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);

/* Element Block Callbacks */
H3D_Export bool Hyper3DReadElements(H3DFileInfo* h3d_file);
H3D_Export bool Hyper3DReadElementsR(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx,
                    H3D_ID subcase_id);
           bool H3DCreateElements(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, 
                    H3D_ID poolname_id, H3D_ElementConfig config, 
                    H3D_ID* connect_ids, H3D_ID comp_id, H3D_ID comp_poolname_id, 
                    H3D_ID node_poolname_id);
           bool H3DCreateElementsMixed(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, 
                    H3D_ID poolname_id, H3D_ElementConfig* configs, 
                    H3D_ID* connect_ids, H3D_ID* comp_id, H3D_ID comp_poolname_id, 
                    H3D_ID node_poolname_id);

/* Elements (second form) */
/*      inode, idof, icoeff, num_inodes are the independant nodes of an      */
/*          element, while dnode, ddof, dcoeff, num_dnodes are the           */
/*          dependant nodes                                                  */
           bool H3DCreateElements2(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, 
                    H3D_ID poolname_id, H3D_ElementConfig config,
                    uint32_t* inode, uint8_t* idof, double* icoeff, uint32_t* inode_cnt,
                    uint32_t* dnode, uint8_t* ddof, double* dcoeff, uint32_t* dnode_cnt, 
                    H3D_ID comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
           bool H3DCreateElements2Mixed(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, 
                    H3D_ID poolname_id, H3D_ElementConfig* configs, 
                    uint32_t* inode, uint8_t* idof, double* icoeff, uint32_t* inode_cnt,
                    uint32_t* dnode, uint8_t* ddof, double* dcoeff, uint32_t* dnode_cnt, 
                    H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);

/* Elements by Pool blocks */
/*  this block is used to specify a element data pool in tabular models      */
/*  this block also expresses the id order that a result data blocks for     */
/*      the named pool will have                                             */
/*  as this block is used for tabluar model only, the element connectivity   */
/*      is to be specified by the position pool index                        */
           bool H3DCreateElementPoolMixed(H3DFileInfo* h3d_file, 
                    uint32_t count, H3D_SIM_IDX sim_idx, H3D_ID subcase_id, 
                    H3D_ID* ids, H3D_ID poolname_id, H3D_ElementConfig* config, 
                    H3D_ID* connect_ids, H3D_ID* comp_id, H3D_ID comp_poolname_id, 
                    H3D_ID node_poolname_id);

/* Property Blocks */
/* * all pool names are specified per block *                                */
H3D_Export bool Hyper3DCountPropShellBlocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountPropShells(H3DFileInfo* h3d_file, uint32_t* counts, 
                    uint32_t* num_pools, uint32_t* num_elems);
H3D_Export bool Hyper3DReadPropShellIDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids);
H3D_Export bool Hyper3DReadPropShellBlock(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids, H3D_ID* mid1, float* thick1,
                    H3D_ID* mid2, float* bk, H3D_ID* mid3, float* thick2, float* mass,
                    float* z1dist, float* z2dist, H3D_ID* mid4, 
                    H3D_ID_POOL_TYPE* ent_type, uint32_t* propPool_cnts, uint32_t* pool_ids, 
                    uint32_t* poolEnt_cnt, uint32_t* ent_ids);

H3D_Export bool Hyper3DCountPropSolidBlocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountPropSolids(H3DFileInfo* h3d_file, uint32_t* counts, 
                    uint32_t* num_pools, uint32_t* num_elems);
H3D_Export bool Hyper3DReadPropSolidIDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids);
H3D_Export bool Hyper3DReadPropSolidBlock(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids, H3D_ID* mids, H3D_ID* cordm, H3D_PROP_ISOP* isop, 
                    H3D_PROP_FCTN* fctn, float* ds, H3D_ID_POOL_TYPE* ent_type, uint32_t* propPool_cnts, 
                    uint32_t* pool_ids, uint32_t* poolElem_cnts, uint32_t* elem_ids);

/* Material Blocks */
/* * all pool names are specified per block *                                */
H3D_Export bool Hyper3DCountMaterial1Blocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountMaterial1s(H3DFileInfo* h3d_file, uint32_t* counts);
H3D_Export bool Hyper3DReadMaterial1IDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids);
H3D_Export bool Hyper3DReadMaterial1Block(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids, float* e, float* g, float* nu, float* rho, 
                    float* a, float* tref, float* ge, float* st, float* sc, float* ss, H3D_ID* sys_id);

H3D_Export bool Hyper3DCountMaterial2Blocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountMaterial2s(H3DFileInfo* h3d_file, uint32_t* counts);
H3D_Export bool Hyper3DReadMaterial2IDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids);
H3D_Export bool Hyper3DReadMaterial2Block(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids, float* gij, float* rho, float* aj, float* tref, 
                    float* ge, float* st, float* sc, float* ss);

H3D_Export bool Hyper3DCountMaterial8Blocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountMaterial8s(H3DFileInfo* h3d_file, uint32_t* counts);
H3D_Export bool Hyper3DReadMaterial8IDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids);
H3D_Export bool Hyper3DReadMaterial8Block(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids, float* e1, float* e2, float* nu, 
                    float* g12, float* g1z, float* g2z, float* rho, float* a1, 
                    float* a2, float* tref, float* xt, float* xc, float* yt, float* yc, 
                    float* s, float* ge, float* f, float* strn);

H3D_Export bool Hyper3DCountMaterial9Blocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountMaterial9s(H3DFileInfo* h3d_file, uint32_t* counts);
H3D_Export bool Hyper3DReadMaterial9IDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids);
H3D_Export bool Hyper3DReadMaterial9Block(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, H3D_ID* ids, float* gij, float* rho, float* ai, 
                    float* tref, float* ge);

/******************************/
/*  Model Atribute Functions  */
/******************************/

/* Graphic Materials Blocks*/
H3D_Export bool Hyper3DCountGraphicMaterialBlocks(H3DFileInfo* h3d_file, uint32_t* count);
H3D_Export bool Hyper3DCountGraphicMaterials(H3DFileInfo* h3d_file, uint32_t* counts);
H3D_Export bool Hyper3DReadGraphicMaterialIDs(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids);
H3D_Export bool Hyper3DReadGraphicMaterialBlock(H3DFileInfo* h3d_file, uint32_t block_idx, 
                    H3D_ID* label_ids, float* alpha, float* ambient, float* diffuse, float* emission, 
                    float* metallic, float* roughness, float* shininess, float* specular);

/* Attribute Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      color - RGB values                                                   */
H3D_Export bool Hyper3DReadAttributes(H3DFileInfo* h3d_file);
           bool H3DCreateAttributes(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddModelAttribute(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_MODEL_ATTRIBS flags, uint8_t color[3], 
                    uint32_t meta_size, const uint8_t* meta_data);
           bool H3DAddAssemblyAttribute(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_ASSEMBLY_ATTRIBS flags, 
                    uint8_t color[3]);
           bool H3DAddComponentAttribute(H3DFileInfo* h3d_file, H3D_ID id,
                    H3D_ID poolname_id, H3D_COMPONENT_ATTRIBS flags, 
                    uint8_t color[3], float feature_angle);
           bool H3DAddComponentAttribute2(H3DFileInfo* h3d_file, H3D_ID id,
                    H3D_ID poolname_id, H3D_COMPONENT_ATTRIBS flags, 
                    uint8_t color[3], H3D_ID gmat_label_id, float feature_angle);
           bool H3DAddSystemAttribute(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SYSTEM_ATTRIBS flags, 
                    uint8_t color[3]);
           bool H3DAddSetAttribute(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_ENTITY_SET_ATTRIBS flags, 
                    uint8_t color[3], uint32_t draw_size);

/* Entity Set Blocks */
/*                                                                           */
H3D_Export bool Hyper3DReadEntitySets(H3DFileInfo* h3d_file);
           bool H3DCreateEntitySets(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID poolname_id);
           bool H3DAddEntitySet(H3DFileInfo* h3d_file, const char* name, 
                    H3D_ID set_id, H3D_ID poolname_id, uint32_t num_pools);
           bool H3DAddEntitySetPool(H3DFileInfo* h3d_file, H3D_ID set_id, 
                    H3D_ID poolname_id, H3D_ID_POOL_TYPE ent_type, 
                    uint32_t num_ids, H3D_ID* ids, bool range);
           bool H3DRemoveEntitySetPool(H3DFileInfo* h3d_file, H3D_ID set_id, 
                    H3D_ID poolname_id, H3D_ID_POOL_TYPE ent_type, 
                    uint32_t num_ids, H3D_ID* ids, bool range);

/* Eroded Element Blocks */
/* * all pool names are specified per block *                                */
H3D_Export bool Hyper3DReadErodedElements(H3DFileInfo* h3d_file, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id);
           bool H3DCreateErodedElements(H3DFileInfo* h3d_file, 
                    uint32_t count, H3D_ID* ids, H3D_ID poolname_id, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id);

/* Mask Blocks */
/* * all pool names are specified per block *                                */
/* * only one entity type allowed per block *                                */
H3D_Export bool Hyper3DReadMasks(H3DFileInfo* h3d_file);
           bool H3DCreateMasks(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DCreateComponentMasks(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
           bool H3DCreateSystemMasks(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
/*  HyperView's default is to mask all markers when imported, thus           */
/*      markers are only unmasked (or made visible)                          */
           bool H3DCreateMarkerMasks(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
           bool H3DCreateEntitySetMasks(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
           bool H3DCreateElementMasks(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
           bool H3DCreateElementRangeMask(H3DFileInfo* h3d_file, 
                    H3D_ID first_id, H3D_ID last_id, H3D_ID poolname_id, H3D_ID view_id);

/* Text Blocks */
H3D_Export bool Hyper3DReadText(H3DFileInfo* h3d_file, const char* text_tag, 
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id);
           bool H3DCreateText(H3DFileInfo* h3d_file, char* text_tag, 
                    uint32_t text_size, char** text_buffer);
           bool H3DAddText(H3DFileInfo* h3d_file, char* text_tag,
                    H3D_SIM_IDX sim_idx, H3D_ID subcase_id,
                    uint32_t text_size, char* text_buffer);

H3D_Export bool Hyper3DReadFATXML(H3DFileInfo* h3d_file);
           bool H3DCreateFATXML(H3DFileInfo* h3d_file, uint32_t text_size, 
                    char** text_buffer);
           bool H3DAddFATXML(H3DFileInfo* h3d_file, uint32_t text_size, 
                    char* text_buffer);


/* View Blocks */
/* Context parameters by type:                                                */
/*  when type == H3D_VIEW_ORTHO,                                              */
/*      context1-context6, clipping planes, xmin,xmax,ymin,max,zmin,zmax      */
/*      context7-context9, unused                                             */
/* when type == H3D_VIEW_PERSPECT                                             */
/*      context1-context6, clipping planes, xmin,xmax,ymin,max,zmin,zmax      */
/*      context7, field of view                                               */
/*      context 8, z offset                                                   */
/*      context9, unused                                                      */
/* when type == H3D_VIEW_LENS                                                 */
/*      context1, perspective angle                                           */
/*      context2, focal length                                                */
/*      context3, sensor height                                               */
/*      context4-contect6, up vector, x, y, z                                 */
/*      context7-context9, offsets, x offset, y offset, z offset              */
H3D_Export bool Hyper3DReadViews(H3DFileInfo* h3d_file);
           bool H3DCreateViews(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddView(H3DFileInfo* h3d_file, char* label, H3D_VIEW_TYPE type,
                    float x, float y, float z, float e1, float e2, float e3,
                    float context1, float context2, float context3, 
                    float context4, float context5, float context6,
                    float context7, float context8, float contex9);
           bool H3DAddView2(H3DFileInfo* h3d_file, char* label, H3D_VIEW_TYPE type,
                    float* view_matrix, uint32_t imagePlaneID, bool multiPt,
                    float context1,  float context2,  float context3, 
                    float context4,  float context5,  float context6,
                    float context7,  float context8,  float contex9,
                    float context10, float context11, float context12,
                    float context13, float context14, float context15,
                    float context16);


/*************************/
/* Result Data Functions */
/*************************/

/* Result data default block */
/*  the purpose of this block is to establish defaults for interpreting      */
/*      the result data contained in this file                               */
/*                                                                           */
/*      count   - please use only 1 at this time                             */
/*      name    - may be NULL                                                */
H3D_Export bool Hyper3DReadResults(H3DFileInfo* h3d_file);
           bool H3DSetResultInfo(H3DFileInfo* h3d_file, const char* name, 
                    H3D_DS_SHELL_METHOD elemSysCalcMethod, uint32_t num_pools);
           bool H3DSetResultSysInfo(H3DFileInfo* h3d_file, H3D_ID_POOL_TYPE type,
                    H3D_ID poolname_id, int sysId);

/* Subcase (Loadcase) Blocks */
/*  animation groups must be defined in order for the reader to determine    */
/*      which vector data types can be animated                              */
/*  HyperView's default is to animate none                                   */
/* Subcase Block Callbacks */
H3D_Export bool Hyper3DReadSubcases(H3DFileInfo* h3d_file);
           bool H3DCreateSubcases(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddSubcase(H3DFileInfo* h3d_file, const char* name, H3D_ID id, H3D_ANALYSIS_TYPE type, 
                    H3D_NODAL_DATA_TYPE format, uint32_t num_datatypes, H3D_ID* dt_ids, 
                    uint32_t num_groups, H3D_ID* anim_grp_id, uint32_t* num_anim_datatypes, 
                    H3D_ID* anim_grp_dt_ids); 

H3D_Export bool Hyper3DReadDerivedSubcases(H3DFileInfo* h3d_file);
           bool H3DCreateDerivedSubcases(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddDerivedSubcase(H3DFileInfo* h3d_file, const char* name, 
                    H3D_ID id, H3D_SUBCASE_TYPE type, uint32_t num_sims, 
                    H3D_ID* subcase_ids, H3D_SIM_IDX* sim_idxs, float* scales);

/* Simulation Blocks */
/* * all time steps in this block are per subcase                            */
/* Simulation Block Callbacks */
H3D_Export bool Hyper3DReadSimulations(H3DFileInfo* h3d_file, H3D_ID subcase_id);
           bool H3DCreateSimulations(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID subcase_id);
           bool H3DAddSimulation(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID subcase_id, const char* name, float syncValue, 
                    H3D_TRIBOOL adaptive, H3D_TRIBOOL eroded);
H3D_Export bool Hyper3DImportValidateSimulation(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx);


/* Datatype Blocks */
/*                                                                           */
/*  an example:                                                              */
/*      2 calculations of Von Mises Stress data to be available, Z1 & Z2     */
/*      create a data type labeled "Stress:Von Mises" with 2 layers          */
/*      the ':' is defined as H3D_DT_DELIMITER                               */
/*      there can be multiple Stress:.... data types, they will be collated  */
/* Datatype Callbacks */
H3D_Export bool Hyper3DReadDatatypes(H3DFileInfo* h3d_file);
           bool H3DCreateDatatypes(H3DFileInfo* h3d_file, uint32_t count);
           bool H3DAddDatatype(H3DFileInfo* h3d_file, const char* name, 
                    H3D_ID id, const char* description, H3D_DS_FORMAT format,
                    H3D_DS_TYPE type, H3D_NF_FORMAT nu_form, uint32_t num_pools);
           bool H3DAddDatatypePoolLayer(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, uint32_t num_layers, H3D_ID* layernames_ids, 
                    bool hasCorners, H3D_TENSOR_TYPE tensor_type, float poisson);

/* Dataset Blocks */
/* * all pool names are specified per block *                                */
/*                                                                           */
/*      num_corners - element centroidal and corner data must be specified   */
/*                      in separate blocks                                   */
/*      num_modes   - should be zero, this field only used by MotionSolve    */
/*      dt_id       - data type id associated with this set                  */
/*      layer_idx   - specific layer index of dt_id of this set              */
/*                      note: H3D_DS_NO_LAYER is specified if there is no    */
/*                            layer associated of the dt_id to this pool     */
/*      data_pname  - pool name where entity IDs are defined                 */
/*      complex data is to be in magnitude/phase form                        */
/*          (i.e. Complex Vector data (MagX, MagY, MagZ, PhX, PhY, PhZ)      */
/* Dataset Callbacks */
H3D_Export bool Hyper3DReadDatasets(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx,
                    H3D_ID subcase_id);
           bool H3DAddDatasetInfo(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID subcase_id, H3D_DS_TYPE type, H3D_DS_FORMAT format, 
                    H3D_NF_FORMAT nu_form, H3D_ID data_poolname_id, 
                    uint32_t set_size, uint32_t num_corners, H3D_ID datatype_id, 
                    int layer_idx, H3D_TENSOR_TYPE tensor_type, float poisson, 
                    H3D_ID comp_id, H3D_ID comp_poolname_id, 
                    uint32_t file_idx, uint32_t fpos, uint32_t fpos2);

H3D_Export bool Hyper3DReadDatasetIDs(H3DFileInfo* h3d_file, 
                    uint32_t file_idx, uint32_t fpos, uint32_t fpos2, 
                    void* client_data);
           bool H3DAddDatasetIDs(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID data_poolname_id, H3D_ID* id, void* client_data);

H3D_Export bool Hyper3DReadDatasetData(H3DFileInfo* h3d_file, 
                    uint32_t file_idx, uint32_t fpos, uint32_t fpos2,
                    void* client_data);
           bool H3DAddDatasetDataBlock(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID data_poolname_id, uint32_t val_size, H3D_ID* id, 
                    float** data, float** data_im, H3D_ID* sys_id, 
                    H3D_ANALYSIS_SYSTEM* system_flag, void* client_data);
           bool H3DAddDatasetDoubleDataBlock(H3DFileInfo* h3d_file, uint32_t count,
                    H3D_ID data_poolname_id, uint32_t val_size, H3D_ID* id, 
                    double** data, double** data_im, H3D_ID* sys_id, 
                    H3D_ANALYSIS_SYSTEM* system_flag, void* client_data);


/* Reading Flex data */
H3D_Export bool Hyper3DReadFlexbodyCount(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID sub_id, void* client_data);
          bool H3DCountFlexbodies(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, H3D_ID* poolname_id, 
                    uint32_t* fb_dt_cnts, H3D_FLEX_TYPE* fb_dts, 
                    uint32_t* ent_pid_cnts, uint32_t* ent_pids, 
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, void* client_data);

H3D_Export bool Hyper3DReadFlexbodyEntityCount(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID sub_id, 
                    H3D_FLEX_TYPE read_type, uint32_t* num_modes,
                    H3D_ID* ent_poolname_id, uint32_t* num_ents);
H3D_Export bool Hyper3DReadFlexbodyEntityIDs(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID sub_id, 
                    H3D_FLEX_TYPE read_type, H3D_ID* data_poolname_id, H3D_ID* ids, 
                    uint32_t buffer_cnt, void* client_data);

H3D_Export bool Hyper3DEnableFastFlexDataRead(H3DFileInfo* h3d_file);
H3D_Export bool Hyper3DReadFlexDataType(H3DFileInfo* h3d_file, H3D_ID id, H3D_ID poolname_id, 
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_FLEX_TYPE read_type, 
                    const uint32_t* block_idxs, uint32_t num_blocks, 
                    H3D_ID* data_ids, uint32_t num_ids, H3D_ID data_ids_pool, 
                    void* buffer, uint32_t buffer_cnt, void* client_data);
           bool H3DFlexAddFlexbodyData(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID sub_id, H3D_ID id, H3D_ID poolname_id, H3D_FLEX_TYPE type, 
                     uint32_t num_items, H3D_ID* data_ids, H3D_ID data_pool_id,
                    void* data, uint32_t data_count, void* client_data);
           bool H3DFlexAddFlexbodyBlockData(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID sub_id, H3D_ID id, H3D_ID poolname_id, 
                    H3D_FLEX_TYPE flex_type, uint32_t block_idx, 
                    uint32_t num_items, H3D_ID* data_ids, H3D_ID data_pool_id, 
                    void* data, uint32_t data_count, void* client_data);

          /* General read to identify all mtxs in file */
H3D_Export bool Hyper3DReadMatrixCount(H3DFileInfo* h3d_file, H3D_MTX_TYPE type,
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, void* client_data);
           bool H3DCountMatrices(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_ID* ids, H3D_ID* poolname_id, 
                    const char** labels, H3D_MTX_TYPE* types,
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, void* client_data);

H3D_Export bool Hyper3DReadMatrix(H3DFileInfo* h3d_file, H3D_ID id, H3D_ID poolname_id,
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, void* client_data);
           bool H3DAddMatrix(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID sub_id, const char* label, H3D_ID id, H3D_ID poolname_id, 
                    H3D_MTX_TYPE type, H3D_MTX_FORMAT format,
                    H3D_MTX_VALUE_FORMAT val_type,
                    uint32_t num_rows, uint32_t num_columns,
                    uint64_t num_nz_count, H3D_ID flexbody_id, 
                    H3D_ID flexbody_poolname_id, void* client_data);
           bool H3DAddMatrixLabel(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                    H3D_ID sub_id, H3D_ID column_id, H3D_ID row_id, H3D_ID label_id, 
                    H3D_ID id, H3D_ID poolname_id, void* client_data);
                    
/* Read API for dense mtx data (by column) */
/*  buffer need only be large enough to hold 1 column of data at a time */
H3D_Export bool Hyper3DReadDenseMtxData(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID sub_id, 
                    const H3D_ID* column_ids, uint32_t num_col_ids, 
                    void** buffer, uint32_t count, void* client_data);
           bool H3DAddDenseMtxData(H3DFileInfo* h3d_file, 
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                    H3D_ID poolname_id, H3D_MTX_TYPE type, 
                    H3D_MTX_FORMAT format, H3D_MTX_VALUE_FORMAT val_type, 
                    H3D_ID column_id, void* buffer, 
                    uint32_t num_rows, void* client_data);

/* Read API for sparse mtx data (by column) */
/*  buffer need only be 'number of columns + 1' in count */
H3D_Export bool Hyper3DReadSparseMtxJaData(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID sub_id, 
                    const H3D_ID* column_ids, uint32_t num_col_ids, 
                    uint32_t* buffer, uint32_t count, 
                    void* client_data);
           bool H3DAddMtxJaData(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                    H3D_ID poolname_id, uint32_t* buffer, void* client_data);

/* retrieves Ia data only */
/*  buffer need only be 'number of rows' in count */
H3D_Export bool Hyper3DReadSparseMtxIaData(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID sub_id, 
                    H3D_ID* column_ids, uint32_t num_col_ids, 
                    uint32_t** IaBuffer, uint32_t count, void* client_data);
           bool H3DAddMtxIaData(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                    H3D_ID poolname_id, H3D_ID column_id, 
                    uint32_t* IaBuffer, void* client_data);

/* retrieves Ia and Aa data as the are data pairs */
H3D_Export bool Hyper3DReadSparseMtxAaData(H3DFileInfo* h3d_file, H3D_ID id, 
                    H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID sub_id, 
                    H3D_ID* column_ids, uint32_t num_col_ids, 
                    uint32_t** IaBuffer, void** AaBuffer, uint32_t count, 
                    void* client_data);
           bool H3DAddMtxAaData(H3DFileInfo* h3d_file, uint32_t count, 
                    H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                    H3D_ID poolname_id, H3D_MTX_TYPE type, 
                    H3D_MTX_FORMAT format, H3D_MTX_VALUE_FORMAT val_type,
                    H3D_ID column_id, uint32_t* IaBuffer, void* AaBuffer, 
                    void* client_data);

/********************************/
/*  H3D callback function table */
/********************************/
typedef struct _H3DReaderFunctionTable
{
    bool (*H3DFileInformation)(H3DFileInfo* h3d_file, 
                 const char* appl_name, const char* creation_date, const char* solver_name, 
                 const char** model_filenames, uint32_t num_models, 
                 const char** result_filenames, uint32_t num_results,
                 const char** comments, uint32_t num_comments); 

    bool (*H3DCreateStrings)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddString)(H3DFileInfo* h3d_file, H3D_ID id, 
                 const char* string);

/***********************************/
/*  Model Data Callback Functions  */
/***********************************/
    bool (*H3DCreateModels)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddModel)(H3DFileInfo* h3d_file, const char* name, H3D_ID id, 
                H3D_TRIBOOL model_is_tabular, H3D_TRIBOOL model_is_adaptive, 
                double quantize_error);

    bool (*H3DCreateAssemblies)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID poolname_id);
    bool (*H3DAddAssemblyToAssembly)(H3DFileInfo* h3d_file,
                const char* name, H3D_ID id, H3D_ID poolname_id, 
                float ix, float iy, float iz, float ie1, float ie2, float ie3, 
                float mx, float my, float mz, float me1, float me2, float me3, 
                H3D_ID parent_id, H3D_ID parent_poolname_id);

    bool (*H3DCreateComponents)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID poolname_id);
    bool (*H3DAddComponentToAssembly)(H3DFileInfo* h3d_file, 
                const char* name, H3D_ID id, H3D_ID poolname_id, 
                float ix, float iy, float iz, float ie1, float ie2, float ie3, 
                float mx, float my, float mz, float me1, float me2, float me3, 
                H3D_ID parent_id, H3D_ID parent_poolname_id, 
                H3D_ID node_poolname_id); 

    bool (*H3DCreatePrimitives)(H3DFileInfo* h3d_file, uint32_t count,
                 H3D_ID poolname_id);
    bool (*H3DAddPrimitiveToAssembly)(H3DFileInfo* h3d_file, const char* name, 
                H3D_ID id,  H3D_ID poolname_id, void* param, 
                float ix, float iy, float iz, float ie1, float ie2, float ie3, 
                float mx, float my, float mz, float me1, float me2, float me3, 
                H3D_ID parent_id, H3D_ID parent_poolname_id); 

    bool (*H3DCountPositionPools2)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, 
                H3D_ID* poolname_ids, uint32_t* num_positions);
    bool (*H3DCreatePositions)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, float** coords, 
                H3D_ID* ids, H3D_ID poolname_id, 
                H3D_ID* refsys_ids, H3D_ID* ansys_ids);
    bool (*H3DCreatePositionPool)(H3DFileInfo* h3d_file, uint32_t num_nodes,
                H3D_ID* ids, H3D_ID poolname_id);

    bool (*H3DCreateElements) (H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, H3D_ID poolname_id, 
                H3D_ElementConfig config, H3D_ID* connect_ids, 
                H3D_ID comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
    bool (*H3DCreateElements2)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, H3D_ID poolname_id, 
                H3D_ElementConfig config, uint32_t* inode, uint8_t* idof, 
                double* icoeff, uint32_t* inode_cnt, uint32_t* dnode, 
                uint8_t* ddof, double* dcoeff, uint32_t* dnode_cnt, 
                H3D_ID comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
    bool (*H3DCreateElementsMixed) (H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, H3D_ID poolname_id,
                H3D_ElementConfig* configs, H3D_ID* connect_ids, 
                H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
    bool (*H3DCreateElements2Mixed)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, H3D_ID poolname_id, 
                H3D_ElementConfig* configs, uint32_t* inode, uint8_t* idof, 
                double* icoeff, uint32_t* inode_cnt, uint32_t* dnode, 
                uint8_t* ddof, double* dcoeff, uint32_t* dnode_cnt, 
                H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
    bool (*H3DCreateElementPoolMixed)(H3DFileInfo* h3d_file, uint32_t count, 
                 H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, H3D_ID poolname_id, 
                 H3D_ElementConfig* config, H3D_ID* connect_ids,  
                 H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);

    bool (*H3DCreatePolygons)(H3DFileInfo* h3d_file, H3D_ID id, 
                uint32_t conn_cnt, uint32_t* count, 
                uint32_t** p_connect, H3D_ID comp_id, H3D_ID comp_poolname_id, 
                H3D_ID elem_poolname_id, H3D_ID node_poolname_id);

    bool (*H3DCountElementPool)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id, H3D_ID* ids, H3D_ID poolname_id, 
                H3D_ElementConfig config,
                H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);
    bool (*H3DCountElementPoolMixed)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx,H3D_ID subcase_id,  H3D_ID* ids, H3D_ID poolname_id, 
                H3D_ElementConfig* configs, 
                H3D_ID* comp_id, H3D_ID comp_poolname_id, H3D_ID node_poolname_id);

    bool (*H3DCreateSystems)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddSystem)(H3DFileInfo* h3d_file, H3D_ID id, H3D_SYSTEM_TYPE type, 
                float global_origin[3], float global_axis[3][3]);

/***************************************/
/*  Model Atribute Callback Functions  */
/***************************************/
    bool (*H3DCreateAttributes)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddModelAttribute)(H3DFileInfo* h3d_file, H3D_ID id,
                H3D_MODEL_ATTRIBS flags, uint8_t color[3], 
                uint32_t meta_size, const uint8_t* meta_data);
    bool (*H3DAddAssemblyAttribute)(H3DFileInfo* h3d_file, H3D_ID id, 
                H3D_ID poolname_id, H3D_ASSEMBLY_ATTRIBS flags, 
                uint8_t color[3]);
    bool (*H3DAddComponentAttribute)(H3DFileInfo* h3d_file, H3D_ID comp_id, 
                H3D_ID poolname_id, H3D_COMPONENT_ATTRIBS flags, 
                uint8_t color[3], float feature_angle);
    bool (*H3DAddComponentAttribute2)(H3DFileInfo* h3d_file, H3D_ID id,
                H3D_ID poolname_id, H3D_COMPONENT_ATTRIBS flags, 
                uint8_t color[3], H3D_ID gmat_label_id, float feature_angle);
    bool (*H3DAddSystemAttribute)(H3DFileInfo* h3d_file, H3D_ID id, 
                 H3D_ID poolname_id, H3D_SYSTEM_ATTRIBS flags, 
                 uint8_t color[3]);
    bool (*H3DAddSetAttribute)(H3DFileInfo* h3d_file, H3D_ID id, H3D_ID pool_id, 
                H3D_ENTITY_SET_ATTRIBS flags, uint8_t color[3], 
                uint32_t draw_size);

    bool (*H3DCreateEntitySets)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID poolname_id);
    bool (*H3DAddEntitySet)(H3DFileInfo* h3d_file, const char* name, 
                H3D_ID set_id, H3D_ID pool_id, uint32_t num_pools);
    bool (*H3DAddEntitySetPool)(H3DFileInfo* h3d_file,  H3D_ID set_id, 
                H3D_ID poolname_id, H3D_ID_POOL_TYPE ent_type, 
                uint32_t num_ids, H3D_ID* ids, bool range);
    bool (*H3DRemoveEntitySetPool)(H3DFileInfo* h3d_file,  H3D_ID set_id, 
                H3D_ID poolname_id, H3D_ID_POOL_TYPE ent_type, 
                uint32_t num_ids, H3D_ID* ids, bool range);

    bool (*H3DCreateErodedElements)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID poolname_id, H3D_SIM_IDX sim_idx, H3D_ID subcase_id);

    bool (*H3DCreateText)(H3DFileInfo* h3d_file, char* text_tag, 
                uint32_t text_size, char** text_buffer);
    bool (*H3DAddText)(H3DFileInfo* h3d_file, char* text_tag,
                H3D_SIM_IDX sim_idx, H3D_ID subcase_id,
                uint32_t text_size, char* text_buffer);
    bool (*H3DCreateFATXML)(H3DFileInfo* h3d_file, uint32_t text_size, 
                char** text_buffer);
    bool (*H3DAddFATXML)(H3DFileInfo* h3d_file, uint32_t text_size, 
                char* text_buffer);

    bool (*H3DCreateViews)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddView)(H3DFileInfo* h3d_file, char* label, H3D_VIEW_TYPE type,
                float x, float y, float z, float e1, float e2, float e3, 
                float context1, float context2, float context3, 
                float context4, float context5, float context6,
                float context7, float context8, float contex9);
    bool (*H3DAddView2)(H3DFileInfo* h3d_file, char* label, H3D_VIEW_TYPE type,
                float* view_matrix, uint32_t imagePlaneID, bool multiPt,
                float context1, float context2, float context3, 
                float context4, float context5, float context6,
                float context7, float context8, float contex9,
                float context10, float context11, float context12,
                float context13, float context14, float context15,
                float context16);

    bool (*H3DCreateMasks)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DCreateComponentMasks)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
    bool (*H3DCreateSystemMasks)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
    bool (*H3DCreateMarkerMasks)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
    bool (*H3DCreateEntitySetMasks)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID poolname_id, H3D_ID view_id);
    bool (*H3DCreateElementMasks)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID* id, H3D_ID poolname_id, H3D_ID view_id);
    bool (*H3DCreateElementRangeMask)(H3DFileInfo* h3d_file, 
                H3D_ID first_id, H3D_ID last_id, H3D_ID poolname_id, H3D_ID view_id);

/**********************************/
/* Result Data Callback Functions */
/**********************************/
    bool (*H3DSetResultInfo)(H3DFileInfo* h3d_file, const char* name, 
                H3D_DS_SHELL_METHOD elemSysCalcMethod, uint32_t num_pools);
    bool (*H3DSetResultSysInfo)(H3DFileInfo* h3d_file, H3D_ID_POOL_TYPE type, 
                H3D_ID poolname_id, int sysId);

    bool (*H3DCreateSubcases)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddSubcase)(H3DFileInfo* h3d_file, const char* name, 
                H3D_ID id, H3D_ANALYSIS_TYPE type, H3D_NODAL_DATA_TYPE format,  
                uint32_t num_datatypes, H3D_ID* dt_ids, 
                uint32_t num_groups, H3D_ID* anim_grp_id, 
                uint32_t* num_anim_datatypes, H3D_ID* anim_grp_dt_ids);
    bool (*H3DCreateDerivedSubcases)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddDerivedSubcase)(H3DFileInfo* h3d_file, const char* name, 
                H3D_ID id, H3D_SUBCASE_TYPE type, uint32_t num_sims, 
                H3D_ID* subcase_ids, H3D_SIM_IDX* sim_idxs, float* scales);

    bool (*H3DCreateSimulations)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID subcase_id);
    bool (*H3DAddSimulation)(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                H3D_ID subcase_id, const char* name, float syncValue, 
                H3D_TRIBOOL adaptive, H3D_TRIBOOL eroded);

    bool (*H3DCreateDatatypes)(H3DFileInfo* h3d_file, uint32_t count);
    bool (*H3DAddDatatype)(H3DFileInfo* h3d_file, const char* name, 
                H3D_ID id, const char* description, 
                H3D_DS_FORMAT format, H3D_DS_TYPE type, H3D_NF_FORMAT nu_form,
                uint32_t num_pools);
    bool (*H3DAddDatatypePoolLayer)(H3DFileInfo* h3d_file, H3D_ID id, 
                H3D_ID poolname_id, uint32_t num_layers, 
                H3D_ID* layernames_ids, bool hasCorners,
                H3D_TENSOR_TYPE tensor_type, float poisson);

    bool (*H3DAddDatasetInfo)(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                H3D_ID subcase_id, H3D_DS_TYPE type, H3D_DS_FORMAT format,
                H3D_NF_FORMAT nu_form, H3D_ID data_poolname_id, 
                uint32_t set_size, uint32_t num_corners, H3D_ID datatype_id, 
                int layer_idx, H3D_TENSOR_TYPE tensor_type, float poisson, 
                H3D_ID comp_id, H3D_ID comp_poolname_id, 
                uint32_t file_idx, uint32_t fpos, uint32_t fpos2);

    bool (*H3DAddDatasetIDs)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID data_poolname_id, H3D_ID* id, void* client_data);

    bool (*H3DAddDatasetDataBlock)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID data_poolname_id, uint32_t val_size, H3D_ID* id, 
                float** data, float** data_im, H3D_ID* sys_id, 
                H3D_ANALYSIS_SYSTEM* system_flag, void* client_data);
    bool (*H3DAddDatasetDoubleDataBlock)(H3DFileInfo* h3d_file, uint32_t count,
                H3D_ID data_poolname_id, uint32_t val_size, H3D_ID* id, 
                double** data, double** data_im, H3D_ID* sys_id, 
                H3D_ANALYSIS_SYSTEM* system_flag, void* client_data);

/*********************************/
/* Flex Data Callback Functions  */
/*********************************/
    bool (*H3DCountFlexbodies)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID* poolname_id, 
                uint32_t* fb_dt_cnts, H3D_FLEX_TYPE* fb_dts, 
                uint32_t* ent_pid_cnts, uint32_t* ent_pids, 
                H3D_SIM_IDX sim_idx, H3D_ID sub_id, void* client_data);
    bool (*H3DFlexAddFlexbodyData)(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                H3D_ID sub_id, H3D_ID id, H3D_ID poolname_id, H3D_FLEX_TYPE flex_type, 
                uint32_t num_items, H3D_ID* data_ids, H3D_ID data_pool_id, 
                void* data, uint32_t data_count, void* client_data);
    bool (*H3DFlexAddFlexbodyBlockData)(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                H3D_ID sub_id, H3D_ID id, H3D_ID poolname_id, 
                H3D_FLEX_TYPE flex_type, uint32_t block_idx, 
                uint32_t num_items, H3D_ID* data_ids, H3D_ID data_pool_id, 
                void* data, uint32_t data_count, void* client_data);

    bool (*H3DCountMatrices)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_ID* ids, H3D_ID* poolname_id, 
                const char** labels, H3D_MTX_TYPE* types,
                H3D_SIM_IDX sim_idx, H3D_ID sub_id, void* client_data);
    bool (*H3DAddMatrix)(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                H3D_ID sub_id, const char* label, H3D_ID id, H3D_ID poolname_id, 
                H3D_MTX_TYPE type, H3D_MTX_FORMAT format,
                H3D_MTX_VALUE_FORMAT val_type,
                uint32_t num_rows, uint32_t num_columns,
                uint64_t num_nz_count, H3D_ID flexbody_id, 
                H3D_ID flexbody_poolname_id, void* client_data);
    bool (*H3DAddMatrixLabel)(H3DFileInfo* h3d_file, H3D_SIM_IDX sim_idx, 
                H3D_ID sub_id, H3D_ID column_id, H3D_ID row_id, H3D_ID label_id, 
                H3D_ID id, H3D_ID poolname_id, void* client_data);
    bool (*H3DAddDenseMtxData)(H3DFileInfo* h3d_file, 
                H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                H3D_ID poolname_id, H3D_MTX_TYPE type,
                H3D_MTX_FORMAT format, H3D_MTX_VALUE_FORMAT val_type, 
                H3D_ID column_id, void* buffer, 
                uint32_t num_rows, void* client_data);
    bool (*H3DAddMtxJaData)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                H3D_ID poolname_id, uint32_t* buffer, void* client_data);
    bool (*H3DAddMtxIaData)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                H3D_ID poolname_id, H3D_ID column_id, uint32_t* IaBuffer, 
                void* client_data);
    bool (*H3DAddMtxAaData)(H3DFileInfo* h3d_file, uint32_t count, 
                H3D_SIM_IDX sim_idx, H3D_ID sub_id, H3D_ID id, 
                H3D_ID poolname_id, H3D_MTX_TYPE type, 
                H3D_MTX_FORMAT format, H3D_MTX_VALUE_FORMAT val_type,
                H3D_ID column_id, uint32_t* IaBuffer, void* AaBuffer, 
                void* client_data);

    /* these are used for any purpose */
    void* user_context1;
    void* user_context2;

} H3DReaderFunctionTable;
H3D_Export bool CreateH3DReaderFunctionTable(H3DFileInfo* h3d_file, H3DReaderFunctionTable*&);
H3D_Export bool Hyper3DSetReaderFuncTable(H3DFileInfo* h3d_file, H3DReaderFunctionTable* ft);
H3D_Export H3DReaderFunctionTable* Hyper3DGetReaderFuncTable(H3DFileInfo* h3d_file);


#if defined (__cplusplus)
}
#endif
