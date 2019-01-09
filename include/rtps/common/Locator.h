/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#ifndef RTPS_LOCATOR_T_H
#define RTPS_LOCATOR_T_H

#include "ucdr/microcdr.h"
#include "rtps/utils/udpUtils.h"

#include <array>

namespace rtps{
    enum class LocatorKind_t : int32_t{
        LOCATOR_KIND_INVALID  = -1,
        LOCATOR_KIND_RESERVED =  0,
        LOCATOR_KIND_UDPv4    =  1,
        LOCATOR_KIND_UDPv6    =  2
    };

    const uint32_t LOCATOR_PORT_INVALID = 0;
    const std::array<uint8_t, 16> LOCATOR_ADDRESS_INVALID = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    struct Locator{
        LocatorKind_t kind = LocatorKind_t::LOCATOR_KIND_INVALID;
        uint32_t port = LOCATOR_PORT_INVALID;
        std::array<uint8_t,16> address = LOCATOR_ADDRESS_INVALID; // TODO make private such that kind and address always match?

        static Locator createUDPv4Locator(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint32_t port){
            Locator locator;
            locator.kind = LocatorKind_t::LOCATOR_KIND_UDPv4;
            locator.address = {0,0,0,0,0,0,0,0,0,0,0,0,a,b,c,d};
            locator.port = port;
            return locator;
        }

        void setInvalid(){
            kind = LocatorKind_t::LOCATOR_KIND_INVALID;
        }

        bool isValid() const{
            return kind != LocatorKind_t::LOCATOR_KIND_INVALID;
        }

        bool readFromUcdrBuffer(ucdrBuffer& buffer){
            if(ucdr_buffer_remaining(&buffer) < sizeof(Locator)){
                return false;
            }else{
                ucdr_deserialize_array_uint8_t(&buffer, reinterpret_cast<uint8_t*>(this), sizeof(Locator));
                return true;
            }
        }

        bool serializeIntoUdcrBuffer(ucdrBuffer& buffer){
            if(ucdr_buffer_remaining(&buffer) < sizeof(Locator)){
                return false;
            }else{
                ucdr_serialize_array_uint8_t(&buffer, reinterpret_cast<uint8_t*>(this), sizeof(Locator));
            }
        }

        ip4_addr_t getIp4Address() const{
            return transformIP4ToU32(address[12], address[13], address[14], address[15]);
        }
    } __attribute__((packed));

    inline Locator getBuiltInUnicastLocator(ParticipantId_t participantId) {
        return Locator::createUDPv4Locator(Config::IP_ADDRESS[0], Config::IP_ADDRESS[1],
                                           Config::IP_ADDRESS[2], Config::IP_ADDRESS[3],
                                           getBuiltInUnicastPort(participantId));
    }

    inline Locator getBuiltInMulticastLocator() {
        return Locator::createUDPv4Locator(239, 255, 0, 1, getBuiltInMulticastPort());
    }

    inline Locator getUserUnicastLocator(ParticipantId_t participantId) {
        return Locator::createUDPv4Locator(Config::IP_ADDRESS[0], Config::IP_ADDRESS[1],
                                           Config::IP_ADDRESS[2], Config::IP_ADDRESS[3],
                                           getUserUnicastPort(participantId));
    }

    inline Locator getUserMulticastLocator() {
        return Locator::createUDPv4Locator(Config::IP_ADDRESS[0], Config::IP_ADDRESS[1],
                                           Config::IP_ADDRESS[2], Config::IP_ADDRESS[3],
                                           getUserMulticastPort());
    }

    inline Locator getDefaultSendMulticastLocator() {
        return Locator::createUDPv4Locator(239, 255, 0, 1,
                                           getBuiltInMulticastPort());
    }
}

#endif //RTPS_LOCATOR_T_H