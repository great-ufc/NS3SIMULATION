/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage, <mathieu.lacage@sophia.inria.fr>
 */

#include <iostream>
#include <fstream>
#include <string>
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/mobility-model.h"
#include "ns3/wifi-mac-header.h"
#include "yans-wifi-channel.h"
#include "yans-wifi-phy.h"
#include "wifi-utils.h"
#include "ns3/node-list.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("YansWifiChannel");

NS_OBJECT_ENSURE_REGISTERED (YansWifiChannel);

TypeId
YansWifiChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::YansWifiChannel")
    .SetParent<Channel> ()
    .SetGroupName ("Wifi")
    .AddConstructor<YansWifiChannel> ()
    .AddAttribute ("PropagationLossModel", "A pointer to the propagation loss model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&YansWifiChannel::m_loss),
                   MakePointerChecker<PropagationLossModel> ())
    .AddAttribute ("PropagationDelayModel", "A pointer to the propagation delay model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&YansWifiChannel::m_delay),
                   MakePointerChecker<PropagationDelayModel> ())
  ;
  return tid;
}

YansWifiChannel::YansWifiChannel ()
{
  NS_LOG_FUNCTION (this);
}

YansWifiChannel::~YansWifiChannel ()
{
  NS_LOG_FUNCTION (this);
  m_phyList.clear ();
}

void
YansWifiChannel::SetPropagationLossModel (const Ptr<PropagationLossModel> loss)
{
  NS_LOG_FUNCTION (this << loss);
  m_loss = loss;
}

void
YansWifiChannel::SetPropagationDelayModel (const Ptr<PropagationDelayModel> delay)
{
  NS_LOG_FUNCTION (this << delay);
  m_delay = delay;
}

void
YansWifiChannel::Send (Ptr<YansWifiPhy> sender, Ptr<const Packet> packet, double txPowerDbm, Time duration) const
{
  NS_LOG_FUNCTION (this << sender << packet << txPowerDbm << duration.GetSeconds ());
  Ptr<MobilityModel> senderMobility = sender->GetMobility ();
  NS_ASSERT (senderMobility != 0);
  for (PhyList::const_iterator i = m_phyList.begin (); i != m_phyList.end (); i++)
    {
      if (sender != (*i))
        {
          //For now don't account for inter channel interference nor channel bonding
          if ((*i)->GetChannelNumber () != sender->GetChannelNumber ())
            {
              continue;
            }

          Ptr<MobilityModel> receiverMobility = (*i)->GetMobility ()->GetObject<MobilityModel> ();
          Time delay = m_delay->GetDelay (senderMobility, receiverMobility);
          double rxPowerDbm = m_loss->CalcRxPower (txPowerDbm, senderMobility, receiverMobility);//
          NS_LOG_DEBUG ("propagation: txPower=" << txPowerDbm << "dbm, rxPower=" << rxPowerDbm << "dbm, " <<
                        "distance=" << senderMobility->GetDistanceFrom (receiverMobility) << "m, delay=" << delay);
          Ptr<Packet> copy = packet->Copy ();
          Ptr<NetDevice> dstNetDevice = (*i)->GetDevice ();
          uint32_t dstNode;
          if (dstNetDevice == 0)
            {
              dstNode = 0xffffffff;
            }
          else
            {
              


              const Address senderaddress =  sender->GetDevice ()->GetAddress();
               const Address receiveraddress =  (*i)->GetDevice ()->GetAddress();



              //---------verifica o atacante-----------//
              std::string attacker1 = ""; 
              std::ifstream myfile ("atacante.txt"); // ifstream = padrão ios:in
              if (myfile.is_open())
              {
                while (! myfile.eof() ) //enquanto end of file for false continua
                {
                  getline (myfile,attacker1); // como foi aberto em modo texto(padrão)
                                         //e não binário(ios::bin) pega cada linha
                 // NS_LOG_UNCOND("Aqui" << attacker1);
                }
                myfile.close();
              }
              if(attacker1 != "")
                attacker1 = "00:00:00:00:00:" + attacker1;
              else
                attacker1 = "ff:00:00:00:00:00";
              //NS_LOG_INFO("YansWifiChannel::Send - " << attacker1);
              char attacker2[attacker1.size()+1];
              const char* attacker3 = strcpy(attacker2, attacker1.c_str());

              
              //S_LOG_UNCOND("MAC Atacante: " << attacker3);
              //--Obtém dados
              if(ns3::Mac48Address::ConvertFrom(receiveraddress) == attacker3){
                
                //Count 
                int count = -1;
                std::ifstream myfile2 ("Values.txt"); // ifstream = padrão ios:in
                std::string stext ="";
                std::string slinha ="";
                if (myfile2.is_open())
                {
                  count = 0;
                  while (! myfile2.eof() ) //enquanto end of file for false continua
                  {
                    getline (myfile2, slinha); // como foi aberto em modo texto(padrão)
                    stext = stext + slinha + "\n";
                    count++;
                    //NS_LOG_UNCOND("YansWifiChannel::Send " << slinha);
                    if(count!=3)
                      slinha = "";
                    
                                 //e não binário(ios::bin) pega cada linha
                  }
                  myfile2.close();
                }
                //NS_LOG_UNCOND("YansWifiChannel::Send " << count);


                //--AP--//
                if((ns3::Mac48Address::ConvertFrom(senderaddress) == "00:00:00:00:00:03" || 
                    ns3::Mac48Address::ConvertFrom(senderaddress) == "00:00:00:00:00:05") 
                    && count == 1) {
                    NodeList nolist;
                    Vector nopos;
                    for (NodeList::Iterator in = nolist.Begin (); in != nolist.End (); in++){
                        if((*in)->GetId() == 1)
                          nopos = (*in)->GetObject<MobilityModel> ()->GetPosition();
                    }
                    NS_LOG_INFO("YansWifiChannel::Send - X: " << nopos.x <<" Y: "<< nopos.y << " Z: " << nopos.z);  
                    NS_LOG_INFO("YansWifiChannel::Send - 4a: " << Simulator::Now() 
                                                                 << " Source: "  << ns3::Mac48Address::ConvertFrom(senderaddress) 
                                                                 << " Destiny: " << ns3::Mac48Address::ConvertFrom(receiveraddress)
                                                                 << " Rxpower "    << rxPowerDbm << " Txpower " << txPowerDbm);
                    NS_LOG_INFO("YansWifiChannel::Send - AP->" << nopos.x << ";;" << nopos.y << ";;" << nopos.z << ";;" << rxPowerDbm <<";;" << txPowerDbm); 
                    //NS_LOG_UNCOND("RXPOWER" << (m_loss->CalcRxPower (txPowerDbm, senderMobility, receiverMobility)));
                    //NS_LOG_UNCOND("position sender " << (senderMobility->GetPosition()));
                    //NS_LOG_UNCOND("position receiver " << (receiverMobility->GetPosition()));
                    //NS_LOG_UNCOND("Distancia calc " <<  senderMobility->GetDistanceFrom (receiverMobility));
                    std::ofstream out("Values.txt");
                    out << stext; 
                    out<< "AP->" << nopos.x << ";;" << nopos.y << ";;" << nopos.z << ";;" << rxPowerDbm << ";;" << txPowerDbm;
                    out.close();  
                    count++;
                } 
                
                //--Drone--//
                if((ns3::Mac48Address::ConvertFrom(senderaddress) == "00:00:00:00:00:04" || 
                    ns3::Mac48Address::ConvertFrom(senderaddress) == "00:00:00:00:00:06") && count > 1 && count <4){
                    NodeList nolist;
                    Vector nopos;
                    for (NodeList::Iterator in = nolist.Begin (); in != nolist.End (); in++){
                        if((*in)->GetId() == 2)
                          nopos = (*in)->GetObject<MobilityModel> ()->GetPosition();
                    }                   
                    //slinha
                    double posx = 0; double posy = 0;
                    if(slinha != ""){
                      std::string delimiter = "->";
                      std::string token = slinha.substr(slinha.find(delimiter)+2, slinha.size());
                      delimiter = ";;";
                      std::stringstream geek(token.substr(0, token.find(delimiter))); 
                       geek >> posx;
                      token = token.substr(token.find(delimiter)+2, token.size());
                      std::stringstream  geek2(token.substr(0, token.find(delimiter))); 
                      geek2 >> posy;
                      //*/
                     // NS_LOG_INFO("YansWifiChannel::Send - Posx: " << posx << " Posy: " << posy);
                    }
                    //NS_LOG_INFO("YansWifiChannel::Send - Drone-> " <<count);
                    //NS_LOG_INFO("YansWifiChannel::Send - Drone-> " << nopos.x << ";;" << nopos.y);
                    //NS_LOG_INFO("YansWifiChannel::Send - Drone-> " << posx+1 << ";;" << posy+1);
                    srand(unsigned(time(NULL)));
                    double xvalue  = (rand() % (25)) + 1;
                    srand(unsigned(time(NULL)));
                    double yvalue  = (rand() % (25)) + 1;
                    //NS_LOG_UNCOND("YansWifiChannel::Send "<< nopos.x << ";;" << nopos.y);
                    if(count==2 || (count==3 && nopos.x > posx+xvalue && nopos.y > posy+yvalue) ){

                      
                      std::ofstream out("Values.txt");
                      out << stext;
                      out << "Drone->" << nopos.x << ";;" << nopos.y << ";;" << nopos.z << ";;" << rxPowerDbm << ";;" << txPowerDbm;
                      if(count==3){               
                        out << "\n" << Simulator::Now().GetSeconds ();
                      }
                      out.close();  
                      count++;

                    NS_LOG_INFO("YansWifiChannel::Send - X: " << nopos.x <<" Y: "<< nopos.y << " Z: " << nopos.z);  
                    NS_LOG_INFO("YansWifiChannel::Send - 4a: " << Simulator::Now() 
                                                                 << " Source: "  << ns3::Mac48Address::ConvertFrom(senderaddress) 
                                                                 << " Destiny: " << ns3::Mac48Address::ConvertFrom(receiveraddress)
                                                                 << " Rxpower "    << rxPowerDbm << " Txpower " << txPowerDbm);
                    //NS_LOG_UNCOND("YansWifiChannel::Send - Drone->" << nopos.x << ";;" << nopos.y << ";;" << nopos.z << ";;" << rxPowerDbm <<";;" << txPowerDbm);
                    //NS_LOG_UNCOND("RXPOWER" << (m_loss->CalcRxPower (txPowerDbm, senderMobility, receiverMobility)));
                    //NS_LOG_UNCOND("position sender " << (senderMobility->GetPosition()));
                    //NS_LOG_UNCOND("position receiver " << (receiverMobility->GetPosition()));
                    //NS_LOG_UNCOND("Distancia calc " <<  senderMobility->GetDistanceFrom (receiverMobility));
                    }
                  }

                }
                else{

                      if((ns3::Mac48Address::ConvertFrom(senderaddress) == "00:00:00:00:00:03" || 
                          ns3::Mac48Address::ConvertFrom(senderaddress) == "00:00:00:00:00:05") && 
                         (ns3::Mac48Address::ConvertFrom(receiveraddress) == "00:00:00:00:00:04" || 
                          ns3::Mac48Address::ConvertFrom(receiveraddress) == "00:00:00:00:00:06"))
                      {
                            std::ofstream out("referenceLoss.txt");
                            out << "AP:DRone->" << rxPowerDbm << ";;" << txPowerDbm << ";;" << senderMobility->GetDistanceFrom (receiverMobility);
                            out.close(); 
                      }
                
              }

              dstNode = dstNetDevice->GetNode ()->GetId ();
            }


          Simulator::ScheduleWithContext (dstNode,
                                          delay, &YansWifiChannel::Receive,
                                          (*i), copy, rxPowerDbm, duration);
        }
    }
}

void
YansWifiChannel::Receive (Ptr<YansWifiPhy> phy, Ptr<Packet> packet, double rxPowerDbm, Time duration)
{
  NS_LOG_FUNCTION (phy << packet << rxPowerDbm << duration.GetSeconds ());
  // Do no further processing if signal is too weak
  // Current implementation assumes constant rx power over the packet duration
  //NS_LOG_UNCOND("Gain " << phy->GetRxGain () << " GetRxSensitivity " << phy->GetRxSensitivity () << " RXPOWER " << rxPowerDbm);
  if ((rxPowerDbm + phy->GetRxGain ()) < phy->GetRxSensitivity ())
    {
      NS_LOG_INFO ("Received signal too weak to process: " << rxPowerDbm << " dBm");
      return;
    }
  phy->StartReceivePreamble (packet, DbmToW (rxPowerDbm + phy->GetRxGain ()), duration);
}

std::size_t
YansWifiChannel::GetNDevices (void) const
{
  return m_phyList.size ();
}

Ptr<NetDevice>
YansWifiChannel::GetDevice (std::size_t i) const
{
  return m_phyList[i]->GetDevice ()->GetObject<NetDevice> ();
}

void
YansWifiChannel::Add (Ptr<YansWifiPhy> phy)
{
  NS_LOG_FUNCTION (this << phy);
  m_phyList.push_back (phy);
}

int64_t
YansWifiChannel::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  int64_t currentStream = stream;
  currentStream += m_loss->AssignStreams (stream);
  return (currentStream - stream);
}

} //namespace ns3
