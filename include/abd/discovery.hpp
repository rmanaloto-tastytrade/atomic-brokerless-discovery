#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <sys/types.h>

namespace abd {

// Represents a published service record for discovery.
struct ServiceRecord {
    pid_t pid;
    uint64_t generation;
    uint64_t capabilities;
    std::string uds_path;
};

// Discovery interface for finding services.
class Discovery {
public:
    // List all available services published in the registry.
    std::vector<ServiceRecord> list_services() const;

    // Wait for a service with a given name. Blocks until available.
    std::optional<ServiceRecord> wait_for_service(const std::string& name) const;
};

// Utilities for publishers to publish their service.
class Publisher {
public:
    // Publish metadata for this service into the discovery registry.
    static bool publish(const ServiceRecord& record);

    // Remove this service's publication (for cleanup).
    static bool unpublish(const std::string& uds_path);
};

} // namespace abd
