// ===============================================================================
// Authors: AFRL/RQQA
// Organization: Air Force Research Laboratory, Aerospace Systems Directorate, Power and Control Division
// 
// Copyright (c) 2017 Government of the United State of America, as represented by
// the Secretary of the Air Force.  No copyright is claimed in the United States under
// Title 17, U.S. Code.  All Other Rights Reserved.
// ===============================================================================

/* 
 * File:   GamsService.h
 * Author: James Edmondson <jedmondson@gmail.com>
 *
 * Created on May 30, 2017, 3:40 PM
 */

#ifndef UXAS_SERVICE_GAMS_SERVICE_H
#define UXAS_SERVICE_GAMS_SERVICE_H


#include "ServiceBase.h"
#include "TypeDefs/UxAS_TypeDefs_String.h"
#include "CallbackTimer.h"

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"
#include "gams/controllers/BaseController.h"
#include "gams/pose/GPSFrame.h"
#include "gams/pose/CartesianFrame.h"


#include "afrl/cmasi/Waypoint.h"
#include "afrl/cmasi/TurnType.h"
#include "afrl/cmasi/MissionCommand.h"

#include <cstdint> // uint32_t

namespace uxas
{
namespace service
{


/*! \class GamsService
 *   \brief A service that provides interfaces to a GAMS-compliant
 *          MADARA knowledge base.
 *
 * 
 * 
 * Configuration String: 
 *  <Service Type="GamsService" />
 * 
 * Options:
 *
 * 
 * Subscribed Messages:
 *  - afrl::cmasi::AirVehicleState
 *  - afrl::cmasi::GroundVehicleState
 *  - uxas::madara::MadaraState
 * 
 * Sent Messages:
 *  - afrl::cmasi::AirVehicleState
 *  - afrl::cmasi::GroundVehicleState
 *  - afrl::cmasi::MissionCommand
 *  - uxas::madara::MadaraState
 */



class GamsService : public ServiceBase
{
public:

    static const std::string&
    s_typeName()
    {
        static std::string s_string("GamsService");
        return (s_string);
    };

    static const std::vector<std::string>
    s_registryServiceTypeNames()
    {
        std::vector<std::string> registryServiceTypeNames = {s_typeName()};
        return (registryServiceTypeNames);
    };
    
    static const std::string&
    s_directoryName()
    {
        static std::string s_string("");
        return (s_string);
    };

    static ServiceBase*
    create()
    {
        return new GamsService;
    };

    GamsService();

    virtual
    ~GamsService();

    
    /** knowledge base pre-configured with UxAS integration **/
    static ::madara::knowledge::KnowledgeBase s_knowledgeBase;
    
    /**
     * Sends a MadaraState message through the existing UxAS system. This
     * is extremely dirty, but I need a way to interact from the
     * UxASMadaraTransport, and I can't think of a different way to
     * interact with the sendLmcpObjectBroadcastMessage protected function.
     * @param  buffer    encoded MADARA character buffer
     * @param  length    length of the character buffer in bytes
     **/
    void sendBuffer (char * buffer, size_t length);
    
    /**
     * Retrieves the UxAS platform for GAMS interactions
     * @return if non-zero, the valid UxAS platform 
     */
    gams::platforms::BasePlatform * getPlatform (void);
    
    /**
     * Sets the logic rate and update send rate.
     * @param hertz      the hertz to execute logic at
     * @param sendHertz  the hertz to send updates at
     **/
    void controllerRun (double hertz, double sendHertz);
    
private:

    static
    ServiceBase::CreationRegistrar<GamsService> s_registrar;

    /** brief Copy construction not permitted */
    GamsService(GamsService const&) = delete;

    /** brief Copy assignment operation not permitted */
    void operator=(GamsService const&) = delete;

    bool
    configure(const pugi::xml_node& serviceXmlNode) override;

    bool
    initialize() override;

    //bool
    //start() override;

    bool
    terminate() override;

    bool
    processReceivedLmcpMessage(std::unique_ptr<uxas::communications::data::LmcpMessage> receivedLmcpMessage) override;

    /// unique agent id (by default generated from ephemeral bindings)
    std::string m_uniqueId;
    
    /// transport settings 
    madara::transport::QoSTransportSettings m_transportSettings;
    
    /// convenience handle to the thread safe context in the knowledge base
    madara::knowledge::ThreadSafeContext * m_context;
    
    /// data received rules, defined in Transport settings
    madara::knowledge::CompiledExpression  m_onDataReceived;
      
    /// monitor for sending bandwidth usage
    madara::transport::BandwidthMonitor   m_sendMonitor;
      
    /// monitor for receiving bandwidth usage
    madara::transport::BandwidthMonitor   m_receiveMonitor;

    /// scheduler for mimicking target network conditions
    madara::transport::PacketScheduler    m_packetScheduler;
    
    /// controller for GAMS agent
    gams::controllers::BaseController *   m_controller;
    
    madara::threads::Threader m_threader;
};

}; //namespace service
}; //namespace uxas

#endif /* UXAS_SERVICE_GAMS_SERVICE_H */

