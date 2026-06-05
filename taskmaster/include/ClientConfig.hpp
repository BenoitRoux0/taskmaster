#ifndef TASKMASTER_CLIENTCONFIG_HPP
# define TASKMASTER_CLIENTCONFIG_HPP

# include <string>
# include <cstdint>

struct ClientConfig {
    std::string host {"localhost"};
    uint16_t    port {12345};

    std::string getBaseUrl() const {
        return "http://" + host + ":" + std::to_string(port);
    }
};

#endif //TASKMASTER_CLIENTCONFIG_HPP