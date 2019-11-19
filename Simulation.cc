/*
Simulação de rede do artigo Técnica de Localização de Dispositivo Atacante usando Veísculos Aéreos não Tripulados
Author : Evilasio Costa Junior
Adptado de ns3/example/tutorial/third.cc
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include <math.h>
//#include "wifinodestatistics.h"
//#include "../src/MyApp.h"


// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1
//                   point-to-point  

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("MytopologyScript");
////====Métodos para posicionamento e trajetória===/////

std::string simulationName;
double xs, ys = 25;
double zs = 0;
double xap, yap, zap = 0.0; //posições do servidor e do ap
double xar, yar, zar = 0.0; //posiççao real do atacante

double xd = 0;
double movatual = 2;
bool direcaoatual = true;
bool verbose = true;

uint32_t attacker;
Ptr<Node> myuav;



//Seta posição do nó
static void
SetPosition (Ptr<Node> node, Vector position)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  mobility->SetPosition (position);
}

//Recupera posição do nó
static Vector
GetPosition (Ptr<Node> node)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  return mobility->GetPosition ();
}


//imprimir posição
void
CourseChange (std::string context, Ptr<const MobilityModel> model)
{
  Vector position = model->GetPosition ();
  NS_LOG_INFO (context <<
    " x = " << position.x << ", y = " << position.y);
}

///mudar podição - Trajetória
static void
AdvancePosition (Ptr<Node> node)
{ 
  double distance = 60;
  Vector pos = GetPosition (node);
  if(pos.x < (xap+distance)){
    //NS_LOG_INFO ("mov atual é:" << movatual);
            if (direcaoatual){
              if(pos.y < yap+distance)
                pos.y += 1.0;
              else{
                if(pos.x < (xd + ((2*distance)/10)*movatual)){
                  pos.x += 1.0;
                }
                else{
                  movatual = movatual+1.0;
                  direcaoatual = false;
                }
              }
            }
            else{
              if(pos.y > yap-distance)
                pos.y -= 1.0;
              else{
                if(pos.x < (xd + ((2*distance)/10)*movatual))
                    pos.x += 1.0;
                else{
                  movatual = movatual+1.0;
                  direcaoatual = true;
                }
              }
            }
  }
  else{
    return;
  }

  SetPosition (node, pos);
 
  //std::string out =  "Endereço : ";//<< node->GetDevice(0)->GetAddress() << "\n";
  //NS_LOG_INFO (out);
  Simulator::Schedule (Seconds (0.1), &AdvancePosition, node);
}


//conversão Decimal para hexadecimal

static std::string DecimalParaHexadecimal(int numeroEntrada){
    int q, resto;
    char hexa[10] = {'\0'}, getInt[10] = {'\0'};
    do{
      q = numeroEntrada / 16;
      resto = numeroEntrada % 16;
      numeroEntrada /= 16;
      
      switch(resto){
        case 10:
          std::strcat(hexa, "A");
        break;
        case 11:
          std::strcat(hexa, "B");
        break;
        case 12:
          std::strcat(hexa, "C");
        break;
        case 13:
          std::strcat(hexa, "D");
        break;
        case 14:
          std::strcat(hexa, "E");
        break;
        case 15:
          std::strcat(hexa, "F");
        break;
        case 16:
          std::strcat(hexa, "G");
        break;
        default:
          std::sprintf(getInt, "%i", resto);
          std::strcat(hexa, getInt);
      }
    }while(q != 0);

    std::string Hexa =  hexa;
    std::string HEXA =  "";
    for(int i=Hexa.size(); i != 0; i--)
       HEXA = HEXA + Hexa[i-1];
    
    return HEXA;
}


//Esse método é aceionado quando o servidor recebe um pacote do atacante

void
RxTraceWithAddresses (std::string path, Ptr<const Packet> packet, const Address &from, const Address &destiny)
{
  int att = attacker-2;
  std::string attstr = std::to_string(att);;
  std::string attacker1 = "10.1.2.";
  attacker1 +=attstr;
  //NS_LOG_UNCOND("teste " << attacker1);
  char attacker2[attacker1.size()+1];
  const char* attacker3 = strcpy(attacker2, attacker1.c_str());

  if(InetSocketAddress::ConvertFrom (from).GetIpv4 () == attacker3){
    //NS_LOG_UNCOND("address:" << from);
    //if (moveuav)

    //cria o arquivo pro referenceloss
    std::ofstream out("referenceLoss.txt");
    out.close();

    //Cria arquivo Atacante
    std::ofstream out2("atacante.txt");
    std::string macatacante = DecimalParaHexadecimal(attacker+4);
    //NS_LOG_UNCOND("address:" << macatacante);
    if(macatacante.size()<2)
      out2 << "0" << macatacante;
    else
      out2 << macatacante;
    //out << attacker+2;
    out2.close();

 
    Simulator::Schedule (Simulator::Now(), &AdvancePosition, myuav); //movimento do uav
  }
  else
    std::cout << "Package From " << InetSocketAddress::ConvertFrom (from).GetIpv4 ()<<"\n";
}


/////====Distance===/// equação de friss
double CalculateDistanceFriss(double signalLevel, double frequency){
  double exp = ((27.55 - (20 * log10(frequency)) + abs(signalLevel))/20); 
  return pow(10.0, exp);
}


////====CalcReferenceLoss====///
double CalcReferenceLoss(double rxPowerDbm, double txPowerDbm, double distance, double expoent=3){
  double referenceLoss = txPowerDbm - rxPowerDbm - 10*expoent*log10(distance);
  return referenceLoss;
}


/////====Distance===/// Log Distance
double CalculateLogModelDistance(double rxPowerDbm, double txPowerDbm, double referenceLoss = 46.6777, double expoent=3, double referenceDistance = 1){
  // Log-distance equation  == rxPowerDbm = txPowerDbm -m_referenceLoss - (10 * m_exponent * log10 (distance / m_referenceDistance))
  // rxPowerDbm = txPowerDbm - m_referenceLoss - 10 * m_expoent * log10(distance) + 10 * m_expoent * log10(m_referenceDistance)
  // log10(distance) = (txPowerDbm  - m_referenceLoss + 10* m_expoent * log10 (m_referenceDistance) - rxPowerDbm)/ 10 * m_expoent
  // distance = pow(10, (txPowerDbm - m_referenceLoss + 10* m_expoent * log10 (m_referenceDistance) - rxPowerDbm)/ 10 * m_expoent)

  // Valores constantes - Podem ser encontrados em propagation/model/propagation-loss-model.cc
  double m_expoent = expoent;
  double m_referenceDistance = referenceDistance;
  double m_referenceLoss = referenceLoss; 

  double exp = (txPowerDbm - m_referenceLoss + 10* m_expoent * log10 (m_referenceDistance) - rxPowerDbm)/ (10 * m_expoent);
  
  return pow(10.0, exp);
}


double* obtainValues(std::string texto, double values[]){
  std::string delimiter = "->";
  std::string token = texto.substr(texto.find(delimiter)+2, texto.size());
  delimiter = ";;";
  std::stringstream geek(token.substr(0, token.find(delimiter))); 
  geek >> values[0];
  token = token.substr(token.find(delimiter)+2, token.size());
  std::stringstream geek1(token.substr(0, token.find(delimiter))); 
  geek1 >> values[1];
  token = token.substr(token.find(delimiter)+2, token.size());
  std::stringstream geek2(token.substr(0, token.find(delimiter))); 
  geek2 >> values[2];
  token = token.substr(token.find(delimiter)+2, token.size());
  std::stringstream geek3(token.substr(0, token.find(delimiter))); 
  geek3 >> values[3];
  token = token.substr(token.find(delimiter)+2, token.size());
  std::stringstream geek4(token); 
  geek4 >> values[4];

  return values;
}//*/

double obtainValue(std::string texto){
  double value = 0;
  std::string delimiter = "->";
  std::stringstream geek(texto.substr(texto.find(delimiter)+2, texto.size())); 
  geek >> value;

  return value;
}

double obtainValuesReferenceLoss(std::string texto){
  double rxPowerDbm, txPowerDbm, distanceAPD = 0;
  std::string delimiter = "->";
  std::string token = texto.substr(texto.find(delimiter)+2, texto.size());
  delimiter = ";;";
  std::stringstream geek(token.substr(0, token.find(delimiter))); 
  geek >> rxPowerDbm;
  token = token.substr(token.find(delimiter)+2, token.size());
  std::stringstream geek2(token.substr(0, token.find(delimiter))); 
  geek2 >> txPowerDbm;
  std::stringstream geek3(token.substr(token.find(delimiter)+2, token.size())); 
  geek3 >> distanceAPD;
  return CalcReferenceLoss(rxPowerDbm, txPowerDbm, distanceAPD);
}//*/




static double* trilateration1(double Dap, double Dd1, double Dd2, 
                             double xap, double yap, double zap,
                             double xd1, double yd1, double zd1,
                             double xd2, double yd2, double zd2,
                             double values[], std::string timefind){
 

  double k1 = 2*(xd1 - xap);
  double l1 = 2*(yd1 - yap);
 // double m1 = 2*(zd1 - zap);                                       
  double k2 = 2*(xd2 - xap);
  double l2 = 2*(yd2 - yap);
 // double m2 = 2*(zd2 - zap);
  double A1 = pow(Dap, 2) - pow(Dd1, 2) - ((pow(xap,2) - pow(xd1,2)) + (pow(yap,2) - pow(yd1,2)));// + (pow(zap,2) - pow(zd1,2)));
  double A2 = pow(Dap, 2) - pow(Dd2, 2) - ((pow(xap,2) - pow(xd2,2)) + (pow(yap,2) - pow(yd2,2)));// + (pow(zap,2) - pow(zd2,2)));
  //double A3 = pow(Dd1, 2) - pow(Dd2, 2) - ((pow(xd1,2) - pow(xd2,2)) + (pow(yd1,2) - pow(yd2,2)) + (pow(zd1,2) - pow(zd2,2)));

  double xa = ((l1*A2) - (l2*A1))/((l1*k2) - (l2*k1));
  double ya = ((l1*A2) - (A1*k2))/((k1*l2) - (k2*l1));


  double za = 0;
  values[0] = xa; values[1] = ya; values[2] = za;

  int count = 0;
  std::string stext, slinha ="";
  std::ifstream resultfile ("Result.txt"); 
  if (resultfile.is_open())
  {
    while (! resultfile.eof() ) //enquanto end of file for false continua
    {
      getline (resultfile, slinha); // como foi aberto em modo texto(padrão)
      stext = stext + slinha + "\n";
      slinha = "";
      count++;
      //NS_LOG_INFO("Mytopology" << stext);
    }
    resultfile.close();

    std::ofstream out2("Result.txt");
    out2 << stext; 
    out2 << "simulation" << count << "--" << timefind << "::" << "Real Attacker->" << xar << ";;" << yar << ";;" << zar << ";;;" 
         << "Attacker Calc position->" << xa << ";;" << ya << ";;" << za;
    out2.close();
  }
  else{
    std::ofstream out("Result.txt"); 
    out << "simulation" << count << "--" << timefind << "::" << "Real Attacker->" << xar << ";;" << yar << ";;" << zar << ";;;" 
        << "Attacker Calc position->" << xa << ";;" << ya << ";;" << za;
    out.close();
  }
  
 
  NS_LOG_INFO ("A posição do atacante é: (" << xar << ", "<< yar <<", " << zar <<")");
  NS_LOG_INFO( "A posição aproximada do Atacante: (" <<  xa <<", " << ya << ", " << za << ")");
  double distance = sqrt(pow((xar-xa),2) + pow((yar - ya),2) + pow((zar - za),2));
  NS_LOG_INFO("Distancia " << distance);
  return values;


}

static void calcLocation(std::string filename){
    std::string svout;
    std::string vlinha[5];
    std::ifstream myfile (filename); // ifstream = padrão ios:in
    int i = 0;
    if (myfile.is_open())
    {
          while (! myfile.eof() ) //enquanto end of file for false continua
          {
            getline (myfile,svout); // como foi aberto em modo texto(padrão)
            if(svout != ""){
                vlinha[i] = svout;
                svout="";
            }
            i++;
          }
         //NS_LOG_INFO("WifiPhy::CalculateTxDuration - ount: " << i);
         myfile.close();
    }
    if (vlinha[3] != ""){
      double values[5], vposition[3];
 
      double vap[3], vd1[3], vd2[3];
      double distanceAP, distanceD1, distanceD2;
      //AP
      obtainValues(vlinha[1], values);
      vap[0] = values[0]; vap[1] = values[1]; vap[2] = values[2];
      double distanceAP1 = sqrt(pow((xar-vap[0]),2) + pow((yar - vap[1]),2) + pow((zar - vap[2]),2));

      //Posicionamento 1 do drone
      obtainValues(vlinha[2], values);
      vd1[0] = values[0]; vd1[1] = values[1]; vd1[2] = values[2]; 
      double distanceD11 = sqrt(pow((xar-vd1[0]),2) + pow((yar - vd1[1]),2) + pow((zar - vd1[2]),2));

      //Posicionamento 2 do drone
      obtainValues(vlinha[3], values);
      vd2[0] = values[0]; vd2[1] = values[1]; vd2[2] = values[2]; 
      double distanceD21 = sqrt(pow((xar-vd2[0]),2) + pow((yar - vd2[1]),2) + pow((zar - vd2[2]),2));

      //NS_LOG_INFO( vd2[0] << " " << vd2[1] << " " << vd2[2] << " " << values[3] << " " << obtainValue(vlinha[0]));


      //----------Calcula Erro --------///
      std::ifstream myfile2 ("referenceLoss.txt"); // ifstream = padrão ios:in
      svout="";
      if (myfile2.is_open())
      {
            int i = 0;
            while (! myfile2.eof()) //enquanto end of file for false continua
            {
              getline (myfile2,svout); // como foi aberto em modo texto(padrão)
              i++;
            }
           //NS_LOG_INFO("WifiPhy::CalculateTxDuration - ount: " << i);
           myfile2.close();
      }


      double referenceLoss = obtainValuesReferenceLoss(svout);

      //----------Calcula Distância das medições ao alco------------------//
      distanceAP = CalculateLogModelDistance(obtainValues(vlinha[1], values)[3], obtainValues(vlinha[1], values)[4], referenceLoss);
      distanceD1 = CalculateLogModelDistance(obtainValues(vlinha[2], values)[3], obtainValues(vlinha[2], values)[4], referenceLoss);
      distanceD2 = CalculateLogModelDistance(obtainValues(vlinha[3], values)[3], obtainValues(vlinha[3], values)[4], referenceLoss);

      

      NS_LOG_INFO("distancias AP : " <<  distanceAP << " Drone pos 1 : " << distanceD1 << " Drone pos 2 : " <<  distanceD2);
      NS_LOG_INFO("distancias reais AP : " <<  distanceAP1 << " Drone pos 1 : " << distanceD11 << " Drone pos 2 : " <<  distanceD21);
      NS_LOG_INFO("Posição AP: (" <<  vap[0] <<", " << vap[1] << ", " << vap[2] << ")");
      NS_LOG_INFO("Posição D1: (" <<  vd1[0] <<", " << vd1[1] << ", " << vd1[2] << ")");
      NS_LOG_INFO("Posição D2: (" <<  vd2[0] <<", " << vd2[1] << ", " << vd2[2] << ")");

      trilateration1(distanceAP, distanceD1, distanceD2, vap[0], vap[1], vap[2], vd1[0], vd1[1], vd1[2], vd2[0], vd2[1], vd2[2], vposition, vlinha[4]);

      //NS_LOG_INFO( "Posição aproximada do Atacante: (" <<  vposition[0] <<", " << vposition[1] << ", " << vposition[2] << ")");
      Simulator::Stop ();

    }
    else
      Simulator::Schedule (Seconds (1.0), &calcLocation, filename);
}



/////=====Simulação======////

int 
main (int argc, char *argv[])
{

  //Entradas da topologia

  uint32_t nWifi = 4;
  bool tracing = false;
  bool printposition = false;
  bool moveuav = false;
  


  NS_LOG_INFO ("Configurando possíveis entradas");
  CommandLine cmd;
  //cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("printposition", "Print uav position", printposition);
  cmd.AddValue ("uavmoviment", "Print uav position", moveuav);
  

  cmd.Parse (argc,argv);


  /// Escolha da estação atacante
  srand(unsigned(time(NULL)));
  attacker  = (rand() % (nWifi-1)) + 3; // nós 0 e 1 são restritos ao server e ao ap
  //std::string attackernode = "O nó atacante é: " + std::to_string(attacker);
  NS_LOG_INFO ("O nó atacante é: " << attacker);
  NS_LOG_INFO ("O nó drone é: " << 2);

  NS_LOG_INFO ("Criando e adicionando dispositivos dos nós p2p");
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  p2pDevices.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  p2pDevices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  NS_LOG_INFO ("Criando e adicionando dispositivos dos nós wifi");

  NodeContainer wifiStaNodes, WifiUavNodes;
  WifiUavNodes.Create (1);
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNodes = p2pNodes.Get (1);
  wifiApNodes.Add(WifiUavNodes.Get(0));
  myuav = WifiUavNodes.Get (0);
  
  
  NS_LOG_INFO ("Construindo os dispositivos Wifi e o canal de interligação entre esses nós");
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default (); //assitente de topologia que cria canal wifi
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default (); //assitente contrução de camada física wifi 
  phy.SetChannel (channel.Create ()); //associa o canal a camada física

  NS_LOG_INFO ("Construindo os MACs");

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager"); //tipo de algoritmo de controle de taxa a usar

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");

    //Dispositivo wifi dos aps
  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNodes);


  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false)); //estações não realizam sondagem ativa
  
  NetDeviceContainer STAAPDevices;
  STAAPDevices = wifi.Install (phy, mac, wifiApNodes);

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);


  NS_LOG_INFO ("Configurando mobilidade dos nós");
  MobilityHelper mobility; // assitente de mobilidade

//anim.SetConstantPosition(p2pNodes.Get(0), xs, ys, zs);

  
  mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                 "X", StringValue("ns3::UniformRandomVariable[Min=50.0|Max=150.0]"),
                                 "Y", StringValue("ns3::UniformRandomVariable[Min=50.0|Max=150.0]")); //alocando posição em um grid

  mobility.Install (p2pNodes); 
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); //sem movimento para os demais nós
  mobility.Install (WifiUavNodes);
  

  //setando arbritariamente posição do server
  Vector pos = GetPosition (p2pNodes.Get(0));
  pos.x = 40.0; pos.y = 70.0; pos.z = 0.0;
  Ptr<MobilityModel> mobilitymodel = p2pNodes.Get(0)->GetObject<MobilityModel> ();
  mobilitymodel->SetPosition (pos);

  //setando arbritariamente posição do ap
  pos.x = 75.0; pos.y = 75.0; pos.z = 0.0;
  mobilitymodel = p2pNodes.Get(1)->GetObject<MobilityModel> ();
  mobilitymodel->SetPosition (pos);
  xap = pos.x;   yap = pos.y;  zap = pos.z;
  NS_LOG_INFO ("A posição do ap é: (" << xap << ", "<< yap <<", " << zap <<")");

  //setando arbritariamente posição do ap
  pos.x = 39.7; pos.y = 39.7; pos.z = 0.0;
  //pos.x = 0.0; pos.y = 0.0; pos.z = 0.0;
  mobilitymodel = WifiUavNodes.Get(0)->GetObject<MobilityModel> ();
  mobilitymodel->SetPosition (pos);
  xd = pos.x;

  pos = GetPosition (wifiStaNodes.Get(attacker-3));
  xar = pos.x; yar = pos.y;  zar = pos.z;


  NS_LOG_INFO ("Aplicando pilha de protocolos");
  InternetStackHelper stack;
  stack.Install (p2pNodes);
  stack.Install (wifiStaNodes);
  stack.Install (WifiUavNodes);


  NS_LOG_INFO ("Gerando IPs");
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0"); //ips p2p
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);


  address.SetBase ("10.1.2.0", "255.255.255.0"); //ipswifi
  address.Assign (staDevices);
  Ipv4InterfaceContainer apInterfaces = address.Assign (apDevices);


  NS_LOG_INFO ("Gerando aplicações");
  ///*
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (29.0));

  UdpEchoClientHelper echoClient (p2pInterfaces.GetAddress (0), 9); 
  echoClient.SetAttribute ("MaxPackets", UintegerValue (25));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes.Get (attacker-3)); //client
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (28.0));//*/




  NS_LOG_INFO ("Gerando tabela de roteamento");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (30.0));

  NS_LOG_INFO ("Gerando rastreamento para as redes");

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcapAll ("second");
    }



  //NS_LOG_INFO ("Executando algoritmo de movimentação e obtendo posicionamento");
  //Simulator::Schedule (Seconds (0.1), &SetInitialPositionUAV, WifiUavNodes.Get (0)); //posição do nó ap
  //if (moveuav)
  //  Simulator::Schedule (Seconds (0.1), &AdvancePosition, WifiUavNodes.Get (0)); //movimento do uav

  std::string filename = "Values.txt";
  //Cria arquivo para Dados de Frequencia, Posição e SignalLevel
  std::ofstream out2(filename);
  out2.close();
  Simulator::Schedule (Seconds (1.0), &calcLocation, filename);
  //Simulator::Schedule (Seconds (0.1), &GetEventIP, p2pNodes.Get (1)); 

  NS_LOG_INFO ("Habilitando impressão do posicionamento do uav");
  if(printposition){
      std::ostringstream oss;
      oss << "/NodeList/" << WifiUavNodes.Get(0)->GetId()<<"/$ns3::MobilityModel/CourseChange";
      Config::Connect (oss.str(), MakeCallback(&CourseChange));
  }

   
  //Register packet receptions to calculate throughput
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/RxWithAddresses",
                   MakeCallback (RxTraceWithAddresses));

  NS_LOG_INFO ("Gerando arquivo para visualizar simulação na GUI NetAnim");
  AnimationInterface anim("mytopology.xml");
  anim.UpdateNodeColor(p2pNodes.Get(0), 0, 0, 255);
  anim.UpdateNodeColor(p2pNodes.Get(1), 0, 0, 255);
  anim.UpdateNodeColor(wifiStaNodes.Get(attacker-3), 0, 0, 0);
  anim.UpdateNodeColor(WifiUavNodes.Get(0), 255, 255, 255);

  Simulator::Run ();
  Simulator::Destroy ();
  std::remove("referenceLoss.txt"); 
  std::remove("atacante.txt"); 
  std::remove("Values.txt");
//*/
  return 0;
}



