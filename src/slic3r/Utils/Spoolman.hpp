#ifndef SLIC3R_SPOOLMAN_HPP
#define SLIC3R_SPOOLMAN_HPP

#include <libslic3r/Color.hpp>

namespace pt = boost::property_tree;

namespace Slic3r {

class SpoolmanVendor;
class SpoolmanFilament;
class SpoolmanSpool;

typedef shared_ptr<SpoolmanVendor>   SpoolmanVendorShrPtr;
typedef shared_ptr<SpoolmanFilament> SpoolmanFilamentShrPtr;
typedef shared_ptr<SpoolmanSpool>    SpoolmanSpoolShrPtr;

/// Contains routines to get the data from the Spoolman server, save as Spoolman data containers, and create presets from them.
/// The Spoolman data classes can only be accessed/instantiated by this class.
/// An instance of this class can only be accessed via the get_instance() function.
class Spoolman
{
    inline static Spoolman* m_instance{nullptr};

    bool m_initialized{false};

    std::map<int, SpoolmanVendorShrPtr>   m_vendors{};
    std::map<int, SpoolmanFilamentShrPtr> m_filaments{};
    std::map<int, SpoolmanSpoolShrPtr>    m_spools{};

    Spoolman()
    {
        m_instance = this;
        m_initialized = pull_spoolman_spools();
    };

    static pt::ptree get_spoolman_json(const string& api_endpoint);
    /// get all the spools from the api and store them
    bool pull_spoolman_spools();

public:
    // returns true if the operation was successful and false for any errors/issues
    static bool create_filament_preset_from_spool(const SpoolmanSpoolShrPtr& spool, const Preset* base_profile);
    // returns true if the operation was successful and false for any errors/issues
    static bool update_filament_preset_from_spool(Preset* filament_preset, bool only_update_statistics = false);

    const std::map<int, SpoolmanSpoolShrPtr>& get_spoolman_spools(bool update = false)
    {
        if (update || !m_initialized)
            m_initialized = pull_spoolman_spools();
        return m_spools;
    };

    SpoolmanSpoolShrPtr get_spoolman_spool_by_id(int spool_id, bool update = false)
    {
        if (update || !m_initialized)
            m_initialized = pull_spoolman_spools();
        return m_spools[spool_id];
    };

    static Spoolman* get_instance()
    {
        if (!m_instance)
            new Spoolman();
        return m_instance;
    };

    friend class SpoolmanVendor;
    friend class SpoolmanFilament;
    friend class SpoolmanSpool;
};

/// Vendor: The vendor name
class SpoolmanVendor
{
public:
    int    id;
    string name;

    void update_from_server();

private:
    Spoolman* m_spoolman;

    explicit SpoolmanVendor(const pt::ptree& json_data) : m_spoolman(Spoolman::m_instance) { update_from_json(json_data); };

    void update_from_json(pt::ptree json_data);
    void apply_to_config(Slic3r::DynamicConfig& config) const;

    friend class Spoolman;
    friend class SpoolmanFilament;
    friend class SpoolmanSpool;
};

/// Filament: Contains data about a type of filament, including the material, weight, price,
/// etc. You can have multiple spools of one type of filament
class SpoolmanFilament
{
public:
    int    id;
    string name;
    string material;
    float  price;
    float  density;
    float  diameter;
    string article_number;
    int    extruder_temp;
    int    bed_temp;
    string color;

    SpoolmanVendorShrPtr m_vendor_ptr;

    void update_from_server(bool recursive = false);

private:
    Spoolman* m_spoolman;

    explicit SpoolmanFilament(const pt::ptree& json_data) : m_spoolman(Spoolman::m_instance)
    {
        m_vendor_ptr = m_spoolman->m_vendors[json_data.get<int>("vendor.id")];
        update_from_json(json_data);
    };

    void update_from_json(pt::ptree json_data);
    void apply_to_config(Slic3r::DynamicConfig& config) const;

    friend class Spoolman;
    friend class SpoolmanVendor;
    friend class SpoolmanSpool;
};

/// Spool: Contains data on the used and remaining amounts of filament
class SpoolmanSpool
{
public:
    int   id;
    float remaining_weight;
    float used_weight;
    float remaining_length;
    float used_length;
    bool  archived;

    SpoolmanFilamentShrPtr m_filament_ptr;

    SpoolmanVendorShrPtr& getVendor() { return m_filament_ptr->m_vendor_ptr; };

    void update_from_server(bool recursive = false);

    void apply_to_config(DynamicConfig& config) const;

private:
    Spoolman* m_spoolman;

    explicit SpoolmanSpool(const pt::ptree& json_data) : m_spoolman(Spoolman::m_instance)
    {
        m_filament_ptr = m_spoolman->m_filaments[json_data.get<int>("filament.id")];
        update_from_json(json_data);
    }

    void update_from_json(pt::ptree json_data);

    friend class Spoolman;
};

} // namespace Slic3r
#endif // SLIC3R_SPOOLMAN_HPP
