// KRATOS  __  __          _    _                _ _           _   _
//        |  \/  | ___  __| |  / \   _ __  _ __ | (_) ___ __ _| |_(_) ___  _ ___
//        | |\/| |/ _ \/ _` | / _ \ | '_ \| '_ \| | |/ __/ _` | __| |/ _ \| '_  |
//        | |  | |  __/ (_| |/ ___ \| |_) | |_) | | | (_| (_| | |_| | (_) | | | |
//        |_|  |_|\___|\__,_/_/   \_\ .__/| .__/|_|_|\___\__,_|\__|_|\___/|_| |_|
//                                  |_|   |_|
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

// System includes
#include <optional>

// External includes

// Project includes
#include "med_inc.h"
#include "med_model_part_io.h"
#include "includes/model_part_io.h"
#include "utilities/parallel_utilities.h"
#include "utilities/variable_utils.h"

namespace Kratos {

namespace {

static const std::map<GeometryData::KratosGeometryType, med_geometry_type> KratosToMedGeometryType {
    { GeometryData::KratosGeometryType::Kratos_Point2D,          MED_POINT1 },
    { GeometryData::KratosGeometryType::Kratos_Point3D,          MED_POINT1 },

    { GeometryData::KratosGeometryType::Kratos_Line2D2,          MED_SEG2 },
    { GeometryData::KratosGeometryType::Kratos_Line3D2,          MED_SEG2 },
    { GeometryData::KratosGeometryType::Kratos_Line2D3,          MED_SEG3 },
    { GeometryData::KratosGeometryType::Kratos_Line3D3,          MED_SEG3 },

    { GeometryData::KratosGeometryType::Kratos_Triangle2D3,      MED_TRIA3 },
    { GeometryData::KratosGeometryType::Kratos_Triangle3D3,      MED_TRIA3 },
    { GeometryData::KratosGeometryType::Kratos_Triangle2D6,      MED_TRIA6 },
    { GeometryData::KratosGeometryType::Kratos_Triangle3D6,      MED_TRIA6 },

    { GeometryData::KratosGeometryType::Kratos_Quadrilateral2D4, MED_QUAD4 },
    { GeometryData::KratosGeometryType::Kratos_Quadrilateral3D4, MED_QUAD4 },
    { GeometryData::KratosGeometryType::Kratos_Quadrilateral2D8, MED_QUAD8 },
    { GeometryData::KratosGeometryType::Kratos_Quadrilateral3D8, MED_QUAD8 },
    { GeometryData::KratosGeometryType::Kratos_Quadrilateral2D9, MED_QUAD9 },
    { GeometryData::KratosGeometryType::Kratos_Quadrilateral3D9, MED_QUAD9 },

    { GeometryData::KratosGeometryType::Kratos_Tetrahedra3D4,    MED_TETRA4 },
    { GeometryData::KratosGeometryType::Kratos_Tetrahedra3D10,   MED_TETRA10 },

    { GeometryData::KratosGeometryType::Kratos_Pyramid3D5,       MED_PYRA5 },
    { GeometryData::KratosGeometryType::Kratos_Pyramid3D13,      MED_PYRA13 },

    { GeometryData::KratosGeometryType::Kratos_Prism3D6,         MED_PENTA6 },
    { GeometryData::KratosGeometryType::Kratos_Prism3D15,        MED_PENTA15 },

    { GeometryData::KratosGeometryType::Kratos_Hexahedra3D8,     MED_HEXA8 },
    { GeometryData::KratosGeometryType::Kratos_Hexahedra3D20,    MED_HEXA20 },
    { GeometryData::KratosGeometryType::Kratos_Hexahedra3D27,    MED_HEXA27 }
};

void CheckMEDErrorCode(const int ierr, const std::string& MEDCallName)
{
    KRATOS_ERROR_IF(ierr < 0) << MEDCallName << " failed with error code " << ierr << "." << std::endl;
}

template<typename T>
void CheckConnectivitiesSize(
    const std::size_t ExpectedSize,
    const std::vector<T>& Conns)
{
    KRATOS_DEBUG_ERROR_IF_NOT(Conns.size() == ExpectedSize) << "Connectivities must have a size of " << ExpectedSize << ", but have " << Conns.size() << "!" << std::endl;
};

template<typename T>
std::function<void(std::vector<T>&)> GetReorderFunction(const med_geometry_type MedGeomType)
{
    switch (MedGeomType)
    {
    case MED_TRIA3:
        return [](auto& Connectivities){
            CheckConnectivitiesSize(3, Connectivities);
            auto t1 = Connectivities[1];
            Connectivities[1] = Connectivities[2];
            Connectivities[2] = t1;
            // std::swap(Connectivities[1], Connectivities[2]);
        };

    case MED_TRIA6:
        KRATOS_ERROR << "Not implemented!" << std::endl;

    case MED_QUAD4:
        return [](auto& Connectivities){
            CheckConnectivitiesSize(4, Connectivities);
            T t1 = Connectivities[1];
            Connectivities[1] = Connectivities[3];
            Connectivities[3] = t1;
            // std::swap(Connectivities[1], Connectivities[3]);
        };

    case MED_QUAD8:
        KRATOS_ERROR << "Not implemented!" << std::endl;

    case MED_QUAD9: // should be same as MED_QUAD8
        KRATOS_ERROR << "Not implemented!" << std::endl;

    case MED_PYRA5:
        KRATOS_ERROR << "Not implemented!" << std::endl;

    case MED_PYRA13:
        KRATOS_ERROR << "Not implemented!" << std::endl;

    default:
        return [](auto& Connectivities){
            // does nothing if no reordering is needed
        };
    }
}

std::string GetKratosGeometryName(
    const med_geometry_type MedGeomType,
    const int Dimension)
{
    switch (MedGeomType)
    {
        case MED_POINT1:
            return Dimension == 2 ? "Point2D" : "Point3D";
        case MED_SEG2:
            return Dimension == 2 ? "Line2D2" : "Line3D2";
        case MED_SEG3:
            return Dimension == 2 ? "Line2D3" : "Line3D3";
        case MED_TRIA3:
            return Dimension == 2 ? "Triangle2D3" : "Triangle3D3";
        case MED_TRIA6:
            return Dimension == 2 ? "Triangle2D6" : "Triangle3D6";
        case MED_QUAD4:
            return Dimension == 2 ? "Quadrilateral2D4" : "Quadrilateral3D4";
        case MED_QUAD8:
            return Dimension == 2 ? "Quadrilateral2D8" : "Quadrilateral3D8";
        case MED_QUAD9:
            return Dimension == 2 ? "Quadrilateral2D9" : "Quadrilateral3D9";
        case MED_TETRA4:
            return "Tetrahedra3D4";
        case MED_TETRA10:
            return "Tetrahedra3D10";
        case MED_PYRA5:
            return "Pyramid3D5";
        case MED_PYRA13:
            return "Pyramid3D13";
        case MED_PENTA6:
            return "Prism3D6";
        case MED_PENTA15:
            return "Prism3D15";
        case MED_HEXA8:
            return "Hexahedra3D8";
        case MED_HEXA20:
            return "Hexahedra3D20";
        case MED_HEXA27:
            return "Hexahedra3D27";
        default:
            KRATOS_ERROR << "MED geometry type " << MedGeomType << " is not available!" << std::endl;
    }
}

typedef std::vector<std::string> StringVectorType;

/// The map containing the id for each component and the corresponding tag
typedef std::unordered_map<IndexType, IndexType> IndexIndexMapType;

/// The map containing the tags and the names of the related submodelparts
typedef std::unordered_map<IndexType, StringVectorType> IndexStringMapType;

/// Auxiliary map
typedef std::unordered_map<IndexType, std::set<IndexType>> IndexIndexSetMapType;

/// Auxiliary map
typedef std::unordered_map<std::set<IndexType>, IndexType, KeyHasherRange<std::set<IndexType>>, KeyComparorRange<std::set<IndexType>> >  IndexSetIndexMapType;

/// Copmutes the collections and assign the tags
void ComputeFamilies(
    const ModelPart& rModelPart,
    IndexIndexMapType& rNodeFamilies,
    IndexIndexMapType& rGeomFamilies,
    IndexStringMapType& rFamilySubModelPartNames
    )
{
    // Initialize and create the auxiliary maps
    IndexIndexSetMapType aux_node_tags, aux_geom_tags;

    // Initialize the collections
    auto insert_tags = [](const auto& rEntities, auto& rAuxTags, const IndexType Tag){
        for(const auto& r_entity : rEntities) {
            rAuxTags[r_entity.Id()].insert(Tag);
        }
    };

    IndexType tag = 1;
    for (const auto& r_smp : rModelPart.SubModelParts()) {
        rFamilySubModelPartNames[tag].push_back(r_smp.Name());

        insert_tags(r_smp.Nodes(), aux_node_tags, tag);
        insert_tags(r_smp.Geometries(), aux_geom_tags, tag);

        ++tag;
    }

    // Now detect all the cases in which an entity belongs to more than one part simultaneously
    IndexSetIndexMapType combinations;

    auto detect_combinations = [&combinations](const auto& rAuxTags){
        for(const auto& r_aux_tag : rAuxTags) {
            const auto& r_value = r_aux_tag.second;
            if (r_value.size() > 1) combinations[r_value] = 0;
        }
    };

   detect_combinations(aux_node_tags);
   detect_combinations(aux_geom_tags);

    // Combinations
    for(auto& combination : combinations) {
        const auto& r_key_set = combination.first;
        for(IndexType it : r_key_set)
            rFamilySubModelPartNames[tag].push_back(rFamilySubModelPartNames[it][0]);
        combinations[r_key_set] = tag;
        ++tag;
    }

    // The final maps are created
    auto assign_families = [&combinations](const auto& rAuxTags, auto& rFamilies){
        for(const auto& [key, r_value] : rAuxTags) {
            if (r_value.size() == 0)
                rFamilies[key] = 0; // Main Model Part
            else if (r_value.size() == 1) // A Sub Model Part
                rFamilies[key] = *r_value.begin();
            else // There is a combination
                rFamilies[key] = combinations[r_value];
        }
    };

    assign_families(aux_node_tags, rNodeFamilies);
    assign_families(aux_geom_tags, rGeomFamilies);

    // Clean up the rFamilySubModelPartNames
    auto uniquify = [](StringVectorType& rVec){
        std::set<std::string> aux_set(rVec.begin(), rVec.end());
        rVec.assign(aux_set.begin(), aux_set.end());
    };

    for (auto& r_smp_names : rFamilySubModelPartNames) {
        uniquify(r_smp_names.second);
    }
}

int GetNumberOfNodes(
    const med_idt FileHandle,
	const char* pMeshName)
{
    KRATOS_TRY

    // indicators if mesh has changed compared to previous step
    // not of interest
    med_bool coordinate_changement;
    med_bool geo_transformation;

    return MEDmeshnEntity(
        FileHandle,
        pMeshName, MED_NO_DT, MED_NO_IT ,
        MED_NODE, MED_NO_GEOTYPE,
        MED_COORDINATE, MED_NO_CMODE,
        &coordinate_changement, &geo_transformation);

    KRATOS_CATCH("")
}

auto GetNodeCoordinates(
    const med_idt FileHandle,
	const char* pMeshName,
    const int NumberOfNodes,
    const int Dimension)
{
    KRATOS_TRY

    std::vector<med_float> coords(NumberOfNodes*Dimension);

    const auto err = MEDmeshNodeCoordinateRd(
        FileHandle, pMeshName,
        MED_NO_DT, MED_NO_IT,
        MED_FULL_INTERLACE,
        coords.data());

    CheckMEDErrorCode(err, "MEDmeshNodeCoordinateRd");

    return coords;

    KRATOS_CATCH("")
}

} // anonymous namespace

class MedModelPartIO::MedFileHandler
{
public:
    MedFileHandler(
        const std::filesystem::path& rFileName,
        const Kratos::Flags Options) :
            mFileName(rFileName)
    {
        KRATOS_TRY

        KRATOS_ERROR_IF(Options.Is(IO::APPEND)) << "Appending to med files is not supported!" << std::endl;
        KRATOS_ERROR_IF(Options.Is(IO::READ) && Options.Is(IO::WRITE)) << "Either reading OR writing is possible, not both!" << std::endl;

        mIsReadMode = Options.IsNot(IO::WRITE);

        // Set the mode (consistent with ModelPartIO)
        // read only by default, unless other settings are specified
        med_access_mode open_mode;

        if (mIsReadMode) {
            open_mode = MED_ACC_RDONLY;

            // check if file exists
            KRATOS_ERROR_IF(!std::filesystem::exists(rFileName)) << "File " << rFileName << " does not exist!" << std::endl;

            // basic checks if the file is compatible with the MED library
            med_bool hdf_ok;
            med_bool med_ok;
            const med_err err = MEDfileCompatibility(rFileName.c_str(), &hdf_ok, &med_ok);

            KRATOS_ERROR_IF(err != 0) << "A problem occured while trying to check the compatibility of file " << rFileName << "!" << std::endl;
            KRATOS_ERROR_IF(hdf_ok != MED_TRUE) << "A problem with HDF occured while trying to open file " << rFileName << "!" << std::endl;
            KRATOS_ERROR_IF(med_ok != MED_TRUE) << "A problem with MED occured while trying to open file " << rFileName << "!" << std::endl;
        } else {
            open_mode = MED_ACC_CREAT;
            mMeshName = "Kratos_Mesh"; // Maybe could use the name of the ModelPart (this is what is displayed in Salome)
        }

        mFileHandle = MEDfileOpen(rFileName.c_str(), open_mode);
        KRATOS_ERROR_IF(mFileHandle < 0) << "A problem occured while opening file " << rFileName << "!" << std::endl;

        if (mIsReadMode) {
            // when reading the mesh, it is necessary to querry more information upfront

            const int num_meshes = MEDnMesh(mFileHandle);
            KRATOS_ERROR_IF(num_meshes != 1) << "Expected one mesh, but file " << mFileName << " contains " << num_meshes << " meshes!" << std::endl;

            mMeshName.resize(MED_NAME_SIZE+1);
            med_int space_dim = MEDmeshnAxis(mFileHandle, 1);
            med_int mesh_dim;
            med_mesh_type mesh_type;
            std::string description(MED_COMMENT_SIZE+1, '\0');
            std::string dt_unit(MED_SNAME_SIZE+1, '\0');
            med_sorting_type sorting_type;
            med_int n_step;
            med_axis_type axis_type;
            std::string axis_name(MED_SNAME_SIZE*space_dim+1, '\0');
            std::string axis_unit(MED_SNAME_SIZE*space_dim+1, '\0');

            med_err err = MEDmeshInfo(
                mFileHandle,
                1,
                mMeshName.data(),
                &space_dim,
                &mesh_dim,
                &mesh_type,
                description.data(),
                dt_unit.data(),
                &sorting_type,
                &n_step,
                &axis_type,
                axis_name.data(),
                axis_unit.data());

            mMeshName.erase(std::find(mMeshName.begin(), mMeshName.end(), '\0'), mMeshName.end());
            mDimension = space_dim;
        }

        KRATOS_CATCH("")
    }

    med_idt GetFileHandle() const
    {
        return mFileHandle;
    }

    const char* GetMeshName() const
    {
        return mMeshName.c_str();
    }

    bool IsReadMode() const
    {
        return mIsReadMode;
    }

    int GetDimension() const
    {
        KRATOS_ERROR_IF_NOT(mDimension.has_value()) << "Dimension can only be querried in read mode!";
        return mDimension.value();
    }

    ~MedFileHandler()
    {
        KRATOS_TRY

        KRATOS_WARNING_IF("MedModelPartIO", MEDfileClose(mFileHandle) < 0) << "Closing of file " << mFileName << " failed!" << std::endl;

        KRATOS_CATCH("")
    }

private:
    std::filesystem::path mFileName;
    med_idt mFileHandle;
    std::string mMeshName;
    bool mIsReadMode;
    std::optional<int> mDimension;
};

MedModelPartIO::MedModelPartIO(const std::filesystem::path& rFileName, const Flags Options)
    : mFileName(rFileName), mOptions(Options)
{
    KRATOS_TRY

    mpFileHandler = Kratos::make_shared<MedFileHandler>(rFileName, Options);

    KRATOS_CATCH("")
}

void MedModelPartIO::ReadModelPart(ModelPart& rThisModelPart)
{
    KRATOS_TRY

    using NodePointerType = ModelPart::NodeType::Pointer;

    KRATOS_ERROR_IF_NOT(mpFileHandler->IsReadMode()) << "MedModelPartIO needs to be created in read mode to read a ModelPart!" << std::endl;

    // reading nodes
    const int num_nodes = GetNumberOfNodes(mpFileHandler->GetFileHandle(), mpFileHandler->GetMeshName());

    if (num_nodes == 0) {
        KRATOS_WARNING("MedModelPartIO") << "Med file " << mFileName << " does not contain any entities!" << std::endl;
        return;
    }

    const int dimension = mpFileHandler->GetDimension();

    const auto node_coords = GetNodeCoordinates(
        mpFileHandler->GetFileHandle(),
        mpFileHandler->GetMeshName(),
        num_nodes,
        dimension);

    std::vector<NodePointerType> new_nodes(num_nodes);

    for (int i=0; i<num_nodes; ++i) {
        std::array<double, 3> coords{0,0,0};
        for (int j=0; j<dimension; ++j) {coords[j] = node_coords[i*dimension+j];}
        new_nodes[i] = rThisModelPart.CreateNewNode(
            i+1,
            coords[0],
            coords[1],
            coords[2]
        );
    }

    KRATOS_INFO("MedModelPartIO") << "Read " << num_nodes << " nodes" << std::endl;

    med_bool coordinatechangement, geotransformation;

    // reading cells
    const int num_geometry_types = MEDmeshnEntity(
        mpFileHandler->GetFileHandle(),
        mpFileHandler->GetMeshName(),
        MED_NO_DT, MED_NO_IT,
        MED_CELL, MED_GEO_ALL,
        MED_CONNECTIVITY, MED_NODAL,
        &coordinatechangement, &geotransformation); // TODO error if smaller zero, holds probably for the other functions too that return med_int

    int num_geometries_total = 0;

    // looping geometry types
    for (int it_geo=1; it_geo<=num_geometry_types; ++it_geo) {
        med_geometry_type geo_type;

        std::string geotypename;
        geotypename.resize(MED_NAME_SIZE +1);

        /* get geometry type */
        med_err err = MEDmeshEntityInfo(
            mpFileHandler->GetFileHandle(),
            mpFileHandler->GetMeshName(),
            MED_NO_DT, MED_NO_IT,
            MED_CELL, it_geo,
            geotypename.data(), &geo_type);

        /* how many cells of type geotype ? */
        const int num_geometries = MEDmeshnEntity(
            mpFileHandler->GetFileHandle(),
            mpFileHandler->GetMeshName(),
            MED_NO_DT, MED_NO_IT,
            MED_CELL, geo_type,
            MED_CONNECTIVITY, MED_NODAL,
            &coordinatechangement, &geotransformation);

        /* read cells connectivity in the mesh */
        const int num_nodes_geo_type = geo_type%100;
        std::vector<med_int> connectivity(num_geometries * num_nodes_geo_type); // TODO use typedef

        err = MEDmeshElementConnectivityRd(
            mpFileHandler->GetFileHandle(),
            mpFileHandler->GetMeshName(),
            MED_NO_DT, MED_NO_IT,
            MED_CELL, geo_type,
            MED_NODAL, MED_FULL_INTERLACE,
            connectivity.data());

        // create geometries
        const std::string kratos_geo_name = GetKratosGeometryName(geo_type, dimension);
        const auto reorder_fct = GetReorderFunction<IndexType>(geo_type);

        std::vector<IndexType> geom_node_ids(num_nodes_geo_type);

        for (std::size_t i=0; i<num_geometries; ++i) {
            for (int j=0; j<num_nodes_geo_type; ++j) {
                const int node_idx = i*num_nodes_geo_type + j;
                geom_node_ids[j] = connectivity[node_idx];
            }
            reorder_fct(geom_node_ids);
            rThisModelPart.CreateNewGeometry(kratos_geo_name, geom_node_ids);
        }

        num_geometries_total += num_geometries;

        KRATOS_INFO("MedModelPartIO") << "Read " << num_geometries << " geometries of type " << kratos_geo_name << std::endl;
    }

    KRATOS_INFO_IF("MedModelPartIO", num_geometries_total > 0) << "Read " << num_geometries_total << " geometries in total" << std::endl;

    KRATOS_CATCH("")
}

void MedModelPartIO::WriteModelPart(const ModelPart& rThisModelPart)
{
    KRATOS_TRY

    // TODO what happens if this function is called multiple times?
    // will it overwrite the mesh?
    // or just crash?

    KRATOS_ERROR_IF(mpFileHandler->IsReadMode()) << "MedModelPartIO needs to be created in write mode to write a ModelPart!" << std::endl;

    MEDfileCommentWr(mpFileHandler->GetFileHandle(), "A 2D unstructured mesh : 15 nodes, 12 cells");

    constexpr med_int dimension = 3; // in Kratos, everything is 3D

    const char axisname[MED_SNAME_SIZE] = "x y";
    const char unitname[MED_SNAME_SIZE] = "cm cm";

    med_err err = MEDmeshCr(
        mpFileHandler->GetFileHandle(),
        mpFileHandler->GetMeshName(), // TODO use name of ModelPart? See comment above, this is what is displayed in Salome TODO check length!
        dimension , //spacedim
        dimension , //meshdim
        MED_UNSTRUCTURED_MESH,
        "Kratos med", // description
        "",
        MED_SORT_DTIT,
        MED_CARTESIAN,
        axisname,
        unitname);

    const std::vector<double> nodal_coords = VariableUtils().GetCurrentPositionsVector<std::vector<double>>(rThisModelPart.Nodes(), dimension);

    if (rThisModelPart.NumberOfNodes() == 0) {
        // TODO warning? like in read
        return;
    }

    // add check and warning if ModelPart is empty
    err = MEDmeshNodeCoordinateWr(
        mpFileHandler->GetFileHandle(),
        mpFileHandler->GetMeshName(),
        MED_NO_DT,
        MED_NO_IT,
        0.0,
        MED_FULL_INTERLACE,
        rThisModelPart.NumberOfNodes(),
        nodal_coords.data());

    if (rThisModelPart.NumberOfGeometries() == 0) {
        return;
    }

    // we deliberately do not care about IDs
    std::unordered_map<int, int> kratos_id_to_med_id;
    int med_id = 1; // starting from 1, since salome cannot parse 0 as node ids
    for (const auto& r_node : rThisModelPart.Nodes()) {
        kratos_id_to_med_id[r_node.Id()] = med_id++;
    }

    using ConnectivitiesType = std::vector<med_int>;
    using ConnectivitiesVector = std::vector<ConnectivitiesType>;
    using FamilyNumbersVector = std::vector<med_int>;

    ConnectivitiesVector connectivities;
    connectivities.reserve(rThisModelPart.NumberOfGeometries()/3); // assuming that three different types of geometries exist

    auto write_geometries = [this](
        const med_geometry_type MedGeomType,
        const std::size_t NumberOfPoints,
        ConnectivitiesVector& Connectivities) {


            const auto reorder_fct = GetReorderFunction<ConnectivitiesType::value_type>(MedGeomType);

            auto GetMedConnectivities = [&reorder_fct](
                const std::size_t NumberOfPoints,
                ConnectivitiesVector& Connectivities) {

                std::vector<med_int> med_conn(Connectivities.size() * NumberOfPoints);

                // flatten and reorder the connectivities
                IndexPartition(Connectivities.size()).for_each([&](const std::size_t i) {
                    reorder_fct(Connectivities[i]);
                    // for (std::size_t p=0; p<NumberOfPoints; ++p) {
                    //     med_conn[i*NumberOfPoints+p] = Connectivities[i][p];
                    // }

                    std::copy(Connectivities[i].begin(), Connectivities[i].end(), med_conn.begin()+(i*NumberOfPoints));
                });

                return med_conn;
            };

        const std::vector<med_int> med_conn = GetMedConnectivities(NumberOfPoints, Connectivities);

        if (MEDmeshElementConnectivityWr (
            mpFileHandler->GetFileHandle(),
            mpFileHandler->GetMeshName(),
            MED_NO_DT, MED_NO_IT , 0.0,
            MED_CELL, MedGeomType ,
            MED_NODAL, MED_FULL_INTERLACE,
            Connectivities.size(), med_conn.data()) < 0) {
                KRATOS_ERROR << "Writing failed!" << std::endl;
        }
    };

    auto write_family_numbers = [this](
        const med_entity_type med_type,
        const med_geometry_type med_geom,
        const FamilyNumbersVector& rFamilyNumbers){
            if (MEDmeshEntityFamilyNumberWr(
                mpFileHandler->GetFileHandle(),
                mpFileHandler->GetMeshName(),
                MED_NO_DT, MED_NO_IT,
                med_type, med_geom,
                rFamilyNumbers.size(), rFamilyNumbers.data())<0){
                    KRATOS_ERROR << "Writing failed!" << std::endl;
                }
    };

    IndexIndexMapType node_family_numbers_map;
    IndexIndexMapType geom_family_numbers_map;
    IndexStringMapType fam_groups;
    ComputeFamilies(rThisModelPart, node_family_numbers_map, geom_family_numbers_map, fam_groups);

    for (const auto& [fam_id, smp_names] : fam_groups) {
        MEDfamilyCr(
            mpFileHandler->GetFileHandle(),
            mpFileHandler->GetMeshName(),
            "asdf", // TODO what to put here???
            fam_id,
            smp_names.size(),
            "smp_names.data()"/*smp_names.data_consecutive*/); // TODO this is wrong!!!
    }

    FamilyNumbersVector node_family_numbers(rThisModelPart.NumberOfNodes(), 0); // zero unless otherwise defined

    const auto nodes_begin = rThisModelPart.NodesBegin();
    IndexPartition(rThisModelPart.NumberOfNodes()).for_each([
        &node_family_numbers_map,
        &node_family_numbers,
        nodes_begin](const std::size_t i) {
            const auto it = node_family_numbers_map.find((nodes_begin+i)->Id());
            if (it != node_family_numbers_map.end()) {
                node_family_numbers[i] = it->second;
            }
    });

    write_family_numbers(MED_NODE, MED_NONE, node_family_numbers);

    std::unordered_map<GeometryData::KratosGeometryType, ConnectivitiesVector> conn_map;
    std::unordered_map<GeometryData::KratosGeometryType, FamilyNumbersVector> fam_map;
    std::unordered_map<GeometryData::KratosGeometryType, int> np_map; // TODO this can be solved better

    for (const auto& r_geom : rThisModelPart.Geometries()) {
        auto this_geom_type = r_geom.GetGeometryType();

        ConnectivitiesType conn;
        for (const auto& r_node : r_geom.Points()) {
            conn.push_back(kratos_id_to_med_id[r_node.Id()]);
        }
        conn_map[this_geom_type].push_back(conn);
        np_map[this_geom_type] = r_geom.PointsNumber();

        const auto fam_it = geom_family_numbers_map.find(r_geom.Id());
        const int fam_id = (fam_it != geom_family_numbers_map.end()) ? fam_it->second : 0;
        fam_map[this_geom_type].push_back(-fam_id); // groups have negative family ids
    }

    // entities of a type have to be written at the same time
    // maybe if opening in append mode it would also work without
    for (auto& [geom_type, conn] : conn_map) {
        const auto med_geom_type = KratosToMedGeometryType.at(geom_type);
        write_geometries(med_geom_type, np_map[geom_type], conn);
        write_family_numbers(MED_CELL, med_geom_type, fam_map[geom_type]);
    }

    KRATOS_CATCH("")
}

void MedModelPartIO::DivideInputToPartitions(SizeType NumberOfPartitions,
                                             const PartitioningInfo& rPartitioningInfo)
{
    // these are not used in ModelPartIO, as the partitioned files are created independently
    std::stringbuf dummy_strbuf;
    auto dummy_stream(Kratos::make_shared<std::iostream>(&dummy_strbuf));

    ModelPartIO(dummy_stream).DivideInputToPartitions(
        NumberOfPartitions,
        rPartitioningInfo);
}

void MedModelPartIO::DivideInputToPartitions(Kratos::shared_ptr<std::iostream> * pStreams,
                                             SizeType NumberOfPartitions,
                                             const PartitioningInfo& rPartitioningInfo)
{
    // these are not used in ModelPartIO, streams are passed from outside
    std::stringbuf dummy_strbuf;
    auto dummy_stream(Kratos::make_shared<std::iostream>(&dummy_strbuf));

    ModelPartIO(dummy_stream).DivideInputToPartitions(
        pStreams,
        NumberOfPartitions,
        rPartitioningInfo);
}

} // namespace Kratos.
