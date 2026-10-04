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

    extern H3DFileInfo* h3d_file;
    extern bool rc;
    extern H3D_ID subcase_id;

    extern unsigned int   model_count;
    extern bool model_tabular;
    extern H3D_TRIBOOL model_adaptive ;
    extern H3D_ID model_id;

    extern H3D_ID assm_poolname_id;


    extern unsigned int assm_count;
    extern H3D_ID assm_id;
    extern H3D_ID model_as_parent;

    extern H3D_ID assm_parent;   // assm_parent = 1

    extern H3D_ID comp_poolname_id;
    extern H3D_ID node_poolname_id;

    extern unsigned int comp_count;
    extern H3D_ID comp_id;
    extern H3D_ID comp_parent_id;      // comp_parent_id = assm 1

    extern H3D_SIM_IDX sim_idx;
    extern H3D_ID rbody_poolname_id;
    extern H3D_ID rbe2_poolname_id;
    extern H3D_ID rbe3_poolname_id;
    extern H3D_ID rwall_poolname_id;
    extern H3D_ID spring_poolname_id;
    extern H3D_ID truss_poolname_id;
    extern H3D_ID elem1D_poolname_id;
    extern H3D_ID elem2D_poolname_id;
    extern H3D_ID sh4n_poolname_id;
    extern H3D_ID sh3n_poolname_id;
    extern H3D_ID shell_poolname_id;
    extern H3D_ID quad_poolname_id;
    extern H3D_ID skin_poolname_id;
    extern H3D_ID solid4n_poolname_id;
    extern H3D_ID solid10n_poolname_id;
    extern H3D_ID solid5n_poolname_id;
    extern H3D_ID solid6n_poolname_id;
    extern H3D_ID solid8n_poolname_id;
    extern H3D_ID solid16n_poolname_id;
    extern H3D_ID solid20n_poolname_id;
    extern H3D_ID rigid_poolname_id;
    extern H3D_ID solid_poolname_id;
    extern H3D_ID onedelem_poolname_id;
    extern H3D_ID beam_poolname_id;
    extern H3D_ID sph_poolname_id;
    extern H3D_ID sphcell_poolname_id;
    extern H3D_ID sphnode_poolname_id;

    extern char edata_type[50];
    extern H3D_ID dt_id;
    extern unsigned int pool_count;
    extern unsigned int layer_count;
    extern bool has_corners;
    extern H3D_TENSOR_TYPE tensor_type; // unused
    extern float poisson;		// default & unused
    extern unsigned int dt_count;
    extern H3D_ID* layername_ids;
