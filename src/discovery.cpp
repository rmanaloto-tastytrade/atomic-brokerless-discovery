#include "abd/discovery.hpp"

namespace abd {

std::vector<ServiceRecord> Discovery::list_services() {
    // TODO: implement discovery
    return {};
}

ServiceRecord Discovery::wait_for_service(std::string_view name) {
    // TODO: implement wait for service
    return {};
}

Publisher::Publisher(const ServiceRecord&) {
    // TODO: implement
}

Publisher::~Publisher() {
    // TODO: cleanup
}

void Publisher::publish() {
    // TODO: implement publish (atomic publish)
}

void Publisher::unpublish() {
    // TODO: implement unpublish
}

} // namespace abd
