#include <cypress/cypress.hpp>

#include "spikingnetwork.h"

#include <cypress/backend/power/power.hpp>
#include <fstream>
#include <iostream>
#include <cypress/util/matrix.hpp>

using json = nlohmann::json;
using namespace cypress;



SpikingNetwork::SpikingNetwork(const char *simulator,size_t numDirections, size_t xSize, size_t ySize, size_t maxAV, size_t maxV)
{
    this->maxAV = maxAV;
    this->maxV = maxV;
    this->xSize = xSize;
    this->ySize = ySize;
    this->numDirections = numDirections;
    runtime = 0;
    this->simulator = simulator;
    parameter = readParameters();
    netw = Network();
    if(this->maxV<numDirections/8) this->maxV = static_cast<size_t>((int)(numDirections/8));
}

void SpikingNetwork::run()
{
    global_logger().min_level(LogSeverity::INFO);
    createNetwork();

    // Run the simulation for "duration" seconds
    cypress::PowerManagementBackend pwbackend(
        cypress::Network::make_backend(simulator));
    netw.run(pwbackend, runtime);
    printResults();
}

int SpikingNetwork::plotPopulation(std::string name, int i, int time){
    std::vector<std::vector<Real>> spikes = {};
    for (size_t j = 0; j < netw.populations(name)[0].size(); j++) {
            spikes.push_back(netw.populations(name)[0][j].signals().data(0));
    }

    pyplot::subplot(3,3,i);
    pyplot::eventplot(spikes);
    pyplot::title(name);
    pyplot::xlabel("Time in ms");
    pyplot::ylabel("Neuron ID");
    pyplot::ylim(-1,int(netw.populations(name)[0].size()));
    pyplot::xlim(0,time);
    return i += 1;
}

int SpikingNetwork::plotMembranVoltage(std::string name, int neuron, int i){
    auto v_and_time = netw.populations<IfCondExp>(name)[0][neuron].signals().get_v();
    std::vector<Real> time, voltage;
    for(size_t j=0; j<v_and_time.rows(); j++){
        time.push_back(v_and_time(j,0));
        voltage.push_back(v_and_time(j,1));
    }
    pyplot::subplot(3,3,i);
    pyplot::plot(time,voltage);
    pyplot::title(("Membran Voltage"+std::to_string(neuron)).c_str());
    pyplot::xlabel("Time in ms");
    pyplot::ylabel("Voltage in mV");
    return i += 1;
}

int SpikingNetwork::plotWeights(int i){
    const auto& weights_ = netw.connection("stdp").connector().learned_weights();
    size_t xSize = netw.populations("X")[0].size();
    size_t ySize = netw.populations("Y")[0].size();
    std::vector<float> weight;
    if(xSize*ySize==weights_.size()){
        for(size_t i=ySize;i>0;i--){
            for(size_t j=0;j<xSize;j++){
                weight.push_back((float)weights_[(i-1)*xSize+j].SynapseParameters[0]);
            }
        }
    }
    float* ptr = weight.data();
    pyplot::subplot(3,3,i);
    pyplot::imshow(ptr,ySize,xSize,1);
    pyplot::title("Learnt Map");
    return i += 1;
}

void SpikingNetwork::printResults()
{
    //maximal 9 Subplots

    // Print the spike times for each target neuron
    pyplot::figure_size(1200, 800);
    int i = 1;

    //i = plotPopulation("triggerHD",i,int(runtime));
    //i = plotPopulation("triggerHDSP",i,int(runtime));
    //i = plotPopulation("triggerHDSN",i,int(runtime));
    //i = plotPopulation("HDSP",i,int(runtime));
    //i = plotPopulation("HDSN",i,int(runtime));
    i = plotPopulation("HD",i,int(runtime));

    //i = plotPopulation("triggerX",i,int(runtime));
    //i = plotPopulation("triggerY",i,int(runtime));
    //i = plotPopulation("triggerMVSP",i,int(runtime));
    //i = plotPopulation("triggerMVSN",i,int(runtime));
    //i = plotPopulation("MVDirPos",i,int(runtime));
    //i = plotPopulation("MVDirNeg",i,int(runtime));
    //i = plotPopulation("XSP",i,int(runtime));
    //i = plotPopulation("XSN",i,int(runtime));
    //i = plotPopulation("YSP",i,int(runtime));
    //i = plotPopulation("YSN",i,int(runtime));
    i = plotPopulation("X",i,int(runtime));
    i = plotPopulation("Y",i,int(runtime));

    i = plotPopulation("POS",i,int(runtime));

    //i = plotPopulation("triggerBS",i,int(runtime));
    i = plotPopulation("CON",i,int(runtime));

    //i= plotMembranVoltage("POS",5,i);

    i = plotWeights(i);

    pyplot::tight_layout();
    pyplot::show();


/*
    std::string file = std::to_string(netw.populations("X")[0].size())+"x"+std::to_string(netw.populations("Y")[0].size());
    std::string file_ = file;
    std::ifstream ifs;

    for(int i=0;i<100;i++){
        ifs = std::ifstream(file_+".png");
        if (ifs.good()) file_ = file+"-"+std::to_string(i);
        else break;
    }

    pyplot::save(file_);
    std::cout<<"file saved"<<std::endl;*/
}

void SpikingNetwork::createIntWTA(std::string name,size_t size,size_t maxShift,bool overflow)
{
    //Create WTA Populations
    netw.create_population<IfCondExp>(size, parameter.neuroParams.parameter(),IfCondExpSignals({"spikes"}),name.c_str());
    //Create Shift Populations
    netw.create_population<IfCondExp>(size*maxShift, parameter.neuroParams.parameter(),IfCondExpSignals({"spikes"}),(name+"SP").c_str());
    netw.create_population<IfCondExp>(size*maxShift, parameter.neuroParams.parameter(),IfCondExpSignals({"spikes"}),(name+"SN").c_str());
    //Create Synapse for WTA
    netw.add_connection(netw.populations(name)[0],netw.populations(name)[0],std::make_unique<AllbutONEConnectorVec>(parameter.wtaSynIn,parameter.delay));
    netw.add_connection(netw.populations(name)[0],netw.populations(name)[0],Connector::one_to_one(parameter.wtaSynEx,parameter.delay));
    //Create Synapse for Shift Layers
    netw.add_connection(netw.populations(name)[0],netw.populations((name+"SN").c_str())[0],std::make_unique<AllbutONEConnectorMat>(parameter.stSynIn,parameter.delay,"x"));
    netw.add_connection(netw.populations((name+"SN").c_str())[0],netw.populations(name)[0],std::make_unique<ShiftConnectorMat>(parameter.wtaSynTrigger,parameter.delay,-1,overflow));
    netw.add_connection(netw.populations(name)[0],netw.populations((name+"SP").c_str())[0],std::make_unique<AllbutONEConnectorMat>(parameter.stSynIn,parameter.delay,"x"));
    netw.add_connection(netw.populations((name+"SP").c_str())[0],netw.populations(name)[0],std::make_unique<ShiftConnectorMat>(parameter.wtaSynTrigger,parameter.delay,1,overflow));

}

void SpikingNetwork::createMoveDirNet(int maxV){

    int numDirections = netw.populations("HD")[0].size();
    int xSize = netw.populations("X")[0].size();
    int ySize = netw.populations("Y")[0].size();

    int x,y;
    netw.create_population<IfCondExp>(numDirections*maxV,parameter.neuroParams.parameter(),IfCondExpSignals({"spikes"}),"MVDirPos");
    netw.create_population<IfCondExp>(numDirections*maxV,parameter.neuroParams.parameter(),IfCondExpSignals({"spikes"}),"MVDirNeg");
    netw.add_connection(netw.populations("HD")[0],netw.populations("MVDirPos")[0],std::make_unique<AllbutONEConnectorMat>(parameter.stSynIn,parameter.delay,"x"));
    netw.add_connection(netw.populations("HD")[0],netw.populations("MVDirNeg")[0],std::make_unique<AllbutONEConnectorMat>(parameter.stSynIn,parameter.delay,"x"));

    //calculates the x and y shift for the heading directions
    std::vector<std::vector<NeuronIndex>> conXPos = {};
    std::vector<std::vector<NeuronIndex>> conXNeg = {};
    std::vector<std::vector<NeuronIndex>> conYPos = {};
    std::vector<std::vector<NeuronIndex>> conYNeg = {};

    std::vector<NeuronIndex> help;
    for(int k=1;k<=maxV;k++){
        for(int i=0;i<numDirections;i++){
            x = (int) round(((float)numDirections/8) * cos(2*M_PI* (float)i/(float)numDirections));
            y = (int) round(((float)numDirections/8) * sin(2*M_PI* (float)i/(float)numDirections));


            help = {};
            for(int j=0;j<xSize;j++){
                    help.push_back(j+(abs(k*x)-1)*xSize);
            }
            if(x>0){
                conXPos.push_back(help);
                conXNeg.push_back({});
            }
            else if(x<0){
                conXPos.push_back({});
                conXNeg.push_back(help);
            }
            else{
                conXPos.push_back({});
                conXNeg.push_back({});
            }


            help = {};
            for(int j=0;j<ySize;j++){
                    help.push_back(j+(abs(k*y)-1)*ySize);
            }
            if(y>0){
                conYPos.push_back(help);
                conYNeg.push_back({});
            }
            else if(y<0){
                conYPos.push_back({});
                conYNeg.push_back(help);
            }

            else{
                conYPos.push_back({});
                conYNeg.push_back({});
            }
        }
    }


    netw.add_connection(netw.populations("MVDirPos")[0],netw.populations("XSP")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conXPos));
    netw.add_connection(netw.populations("MVDirPos")[0],netw.populations("XSN")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conXNeg));
    netw.add_connection(netw.populations("MVDirPos")[0],netw.populations("YSP")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conYPos));
    netw.add_connection(netw.populations("MVDirPos")[0],netw.populations("YSN")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conYNeg));
    netw.add_connection(netw.populations("MVDirNeg")[0],netw.populations("XSN")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conXPos));
    netw.add_connection(netw.populations("MVDirNeg")[0],netw.populations("XSP")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conXNeg));
    netw.add_connection(netw.populations("MVDirNeg")[0],netw.populations("YSN")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conYPos));
    netw.add_connection(netw.populations("MVDirNeg")[0],netw.populations("YSP")[0],std::make_unique<CustomConnector>(parameter.stSynEx,parameter.delay,conYNeg));

}

parameterType SpikingNetwork::readParameters()
{

        // Reading the configuration file
        Json config = readFromFile("../config/parameters.json");

        parameterType parameter;

        // Parsing NeuronParameters
        parameter.neuroParams = NeuronParameter(IfCondExp::inst(), config["neuron_params"]);
        parameter.neuroParams2 = NeuronParameter(IfCondExp::inst(), config["neuron_params_2"]);
        parameter.neuroParams3 = NeuronParameter(IfCondExp::inst(), config["neuron_params_3"]);
        // Common Parameters
        parameter.delay = config["common"]["delay"];
        parameter.start = config["common"]["start"];
        // WTA Parameters
        parameter.wtaSynEx = config["wtaParams"]["synEx"];
        parameter.wtaSynIn = config["wtaParams"]["synIn"];
        parameter.wtaSynTrigger = config["wtaParams"]["synTrigger"];
        // Shift Trigger Parameters
        parameter.stSynEx = config["stParams"]["synEx"];
        parameter.stSynIn = config["stParams"]["synIn"];
        // Plastic Synapse Parameters
        parameter.psWeight = config["psParams"]["weight"];
        parameter.psDelay = config["psParams"]["delay"];
        parameter.psAMinus = config["psParams"]["A_minus"];
        parameter.psAPlus = config["psParams"]["A_plus"];
        parameter.psTauMinus = config["psParams"]["tau_minus"];
        parameter.psTauPlus = config["psParams"]["tau_plus"];
        parameter.psWMin = config["psParams"]["w_min"];
        parameter.psWMax = config["psParams"]["w_max"];
        // Other Parameters
        parameter.conSynEx = config["others"]["conSynEx"];
        parameter.conDelay = config["others"]["conDelay"];
        parameter.posSynEx = config["others"]["posSynEx"];

        return parameter;
}

void SpikingNetwork::createPositionNetwork()
{ //idea: ix and iy neurons inhibit all x and y positions in Position Network except their own mirrors, noise lets the final positon neuron spike
    //create position population
    netw.create_population<IfCondExp>(netw.populations("X")[0].size()*netw.populations("Y")[0].size(), parameter.neuroParams2.parameter(),IfCondExpSignals({"spikes"}).record_v(),"POS");
    //add synapse
    netw.add_connection(netw.populations("X")[0],netw.populations("POS")[0],std::make_unique<OneToVecConnector>(parameter.posSynEx,parameter.delay,"x"));
    netw.add_connection(netw.populations("Y")[0],netw.populations("POS")[0],std::make_unique<OneToVecConnector>(parameter.posSynEx,parameter.delay,"y"));
}

void SpikingNetwork::createCollisionDetectionNetwork()
{
    //Create Populations
    netw.create_population<IfCondExp>(1, parameter.neuroParams3.parameter(),IfCondExpSignals({"spikes"}).record_v(),"CON"); //collision neuron

    //connect Position Network to CON
    SpikePairRuleAdditive plasctic_synapse = SpikePairRuleAdditive().weight(parameter.psWeight).delay(parameter.psDelay)
            .A_minus(parameter.psAMinus).A_plus(parameter.psAPlus).tau_minus(parameter.psTauMinus)
            .tau_plus(parameter.psTauPlus).w_max(parameter.psWMax).w_min(parameter.psWMin);
    netw.add_connection(netw.populations("POS")[0],netw.populations("CON")[0],Connector::all_to_all(plasctic_synapse),"stdp");
}

void SpikingNetwork::createTrigger(std::string name,size_t maxShift,std::vector<triggerType> trigger)
{
    std::vector<std::vector<Real>> spikeTimesSP_({});
    std::vector<std::vector<Real>> spikeTimesSN_({});
    for (size_t i=0;i<maxShift;i++){
        spikeTimesSP_.push_back({});
        spikeTimesSN_.push_back({});
    }
    for(size_t i=1;i<trigger.size();i++){ //no shift at start
        if(trigger[i].value>0) spikeTimesSP_[trigger[i].value-1].push_back(trigger[i].startTime);
        else if(trigger[i].value<0) spikeTimesSN_[abs(trigger[i].value)-1].push_back(trigger[i].startTime);
    }
    std::vector<SpikeSourceArrayParameters> spikeTimesSP;
    std::vector<SpikeSourceArrayParameters> spikeTimesSN;
    for (size_t i=0;i<maxShift;i++){
        spikeTimesSP.push_back(SpikeSourceArrayParameters(spikeTimesSP_[i]));
        spikeTimesSN.push_back(SpikeSourceArrayParameters(spikeTimesSN_[i]));
    }
    netw.create_population<SpikeSourceArray>(maxShift, spikeTimesSP, SpikeSourceArraySignals({"spikes"}),("trigger"+name+"SP").c_str());
    netw.create_population<SpikeSourceArray>(maxShift, spikeTimesSN,SpikeSourceArraySignals({"spikes"}),("trigger"+name+"SN").c_str());
}

void SpikingNetwork::createTrigger(std::string name,std::vector<triggerType2> trigger)
{
    std::vector<Real> spikeTimes_;
    for(size_t i=1;i<trigger.size();i++){
        for(size_t j=0; j<trigger[i].value.size();j++){
            if(trigger[i].value[j]==true) spikeTimes_.push_back(trigger[i].startTime+parameter.conDelay);
            //break;
        }
    }
    SpikeSourceArrayParameters spikeTimes = SpikeSourceArrayParameters(spikeTimes_);
    netw.create_population<SpikeSourceArray>(1, spikeTimes, SpikeSourceArraySignals({"spikes"}),("trigger"+name).c_str());
/*
    std::vector<std::vector<Real>> spikeTimes_;
    for (size_t i=0;i<trigger[0].value.size();i++){
        spikeTimes_.push_back({});
    }
    for(size_t i=1;i<trigger.size();i++){
        for(size_t j=0; j<trigger[i].value.size();j++){
            if(trigger[i].value[j]==true) spikeTimes_[j].push_back(trigger[i].startTime+8);
        }
    }
    std::vector<SpikeSourceArrayParameters> spikeTimes;
    for (size_t i=0;i<trigger[0].value.size();i++){
        spikeTimes.push_back(SpikeSourceArrayParameters(spikeTimes_[i]));
    }
    netw.create_population<SpikeSourceArray>(trigger[0].value.size(), spikeTimes, SpikeSourceArraySignals({"spikes"}),("trigger"+name).c_str());
*/}

void SpikingNetwork::createNetwork()
{
    //load data
    json simulationResults = readFromFile("../result/simulation");
    std::vector<triggerType> av;
    std::vector<triggerType> v;
    std::vector<triggerType2> b;
    std::vector<bool> bs;
    runtime = (Real) simulationResults["simulationTime"] + 10;
    for(int i=0; i<=runtime ;i++){
        if(simulationResults[std::to_string(i)] != nullptr){
            v.push_back({simulationResults[std::to_string(i)]["v"],i});
            av.push_back({simulationResults[std::to_string(i)]["av"],i});
            bs ={};
            for(int j=1;;j++){
                if(simulationResults[std::to_string(i)]["bumper"][std::to_string(j)] == nullptr) break;
                else{
                    bs.push_back(simulationResults[std::to_string(i)]["bumper"][std::to_string(j)]);
                }
            }
            b.push_back({bs,i});
        }
    }
    netw = Network();

    createIntWTA("HD",numDirections,maxAV,true);
    createIntWTA("X",xSize,maxV,false);
    createIntWTA("Y",ySize,maxV,false);
    createPositionNetwork();
    createCollisionDetectionNetwork();
    createMoveDirNet((int)maxV);

    createTrigger("HD",maxAV,av);
    createTrigger("MV",maxV,v);
    createTrigger("BS",b);

    //Create Initial Trigger (for hd in timestep zero to initialize starting direction)
    netw.create_population<SpikeSourceArray>(1,SpikeSourceArrayParameters(std::vector<cypress::Real> {parameter.start}),SpikeSourceArraySignals({"spikes"}),"triggerHD");
    netw.add_connection(netw.populations("triggerHD")[0],netw.populations("HD")[0],std::make_unique<CustomConnector>(parameter.wtaSynTrigger,parameter.delay,std::vector<std::vector<NeuronIndex>> {{int(simulationResults["0"]["dir"])}}));
    //Create Initial Trigger (for x in timestep zero to initialize starting direction)
    netw.create_population<SpikeSourceArray>(1,SpikeSourceArrayParameters(std::vector<cypress::Real> {parameter.start}),SpikeSourceArraySignals({"spikes"}),"triggerX");
    netw.add_connection(netw.populations("triggerX")[0],netw.populations("X")[0],std::make_unique<CustomConnector>(parameter.wtaSynTrigger,parameter.delay,std::vector<std::vector<NeuronIndex>> {{int(simulationResults["0"]["x"])}}));
    //Create Initial Trigger (for y in timestep zero to initialize starting direction)
    netw.create_population<SpikeSourceArray>(1,SpikeSourceArrayParameters(std::vector<cypress::Real> {parameter.start}),SpikeSourceArraySignals({"spikes"}),"triggerY");
    netw.add_connection(netw.populations("triggerY")[0],netw.populations("Y")[0],std::make_unique<CustomConnector>(parameter.wtaSynTrigger,parameter.delay,std::vector<std::vector<NeuronIndex>> {{int(simulationResults["0"]["y"])}}));

    //Creating Synapse for HD Trigger
    netw.add_connection(netw.populations("triggerHDSN")[0],netw.populations("HDSN")[0],std::make_unique<OneToVecConnector>(parameter.stSynEx,parameter.delay,"y"));
    netw.add_connection(netw.populations("triggerHDSP")[0],netw.populations("HDSP")[0],std::make_unique<OneToVecConnector>(parameter.stSynEx,parameter.delay,"y"));

    //Creating Synapse for Shift Trigger
    netw.add_connection(netw.populations("triggerMVSN")[0],netw.populations("MVDirNeg")[0],std::make_unique<OneToVecConnector>(parameter.stSynEx,parameter.delay,"y"));
    netw.add_connection(netw.populations("triggerMVSP")[0],netw.populations("MVDirPos")[0],std::make_unique<OneToVecConnector>(parameter.stSynEx,parameter.delay,"y"));

    //Creating Synapse
    netw.add_connection(netw.populations("triggerBS")[0],netw.populations("CON")[0],Connector::all_to_all(parameter.conSynEx,parameter.delay));

    // Calls graphviz to create visualization of the network, see graph.dot.pdf
    //create_dot(netw, "Simple Network");
}

json SpikingNetwork::readFromFile(std::string directory){

    std::ifstream file;
    file.open(directory, std::ifstream::in);
    if (!file.good()) throw std::runtime_error("Could not open file");
    json j;
    file >> j;
    return j;
}

AllbutONEConnectorMat::AllbutONEConnectorMat(Real weight, Real delay, std::string direction, bool self_connections) : UniformConnector(weight,delay,self_connections)
{
    this->direction = direction;
}

void AllbutONEConnectorMat::connect(const ConnectionDescriptor &descr,std::vector<LocalConnection> &tar) const
{
        NeuronIndex start_src = descr.nid_src0();
        NeuronIndex start_tar = descr.nid_tar0();
        NeuronIndex end_src = descr.nid_src1();
        NeuronIndex end_tar = descr.nid_tar1();
        NeuronIndex len_src = end_src - start_src; //number of neurons in src
        NeuronIndex len_tar = end_tar - start_tar; //number of neurons in tar
        NeuronIndex vectorsize = len_tar/len_src;

        if(direction=="x"){           
            for (NeuronIndex ind = 0; ind < len_src; ind++){
                for (NeuronIndex ind2 = 0; ind2 < len_src; ind2++ ){
                    for(NeuronIndex ind3 = 0; ind3<vectorsize;ind3++){
                        if(ind2!=ind){
                            tar.emplace_back(start_src+ind, start_tar+ind2+ind3*len_src, *m_synapse);
                        }
                    }
                }

            }
        }
        else if(direction=="y"){
            for (NeuronIndex ind = 0; ind < len_src; ind++) {
                for (NeuronIndex n_tar = start_tar; n_tar < end_tar; n_tar++) {
                    if((n_tar < vectorsize*ind+start_tar)||(n_tar >= vectorsize*(ind+1)+start_tar)){
                        tar.emplace_back(start_src+ind, n_tar, *m_synapse);
                    }
                }
            }
        }

}

void AllbutONEConnectorVec::connect(const ConnectionDescriptor &descr,std::vector<LocalConnection> &tar) const
{
    for (NeuronIndex n_src = descr.nid_src0(); n_src < descr.nid_src1(); n_src++) {
        for (NeuronIndex n_tar = descr.nid_tar0(); n_tar < descr.nid_tar1(); n_tar++) {
            if (n_src != n_tar) {
                tar.emplace_back(n_src, n_tar, *m_synapse);
            }
        }
    }
}

ShiftConnector::ShiftConnector(Real weight, Real delay, signed shift, bool overflow, bool self_connections) : UniformConnector(weight,delay,self_connections)
{
    this->shift = shift;
    this->overflow = overflow;
}

void ShiftConnector::connect(const ConnectionDescriptor &descr,std::vector<LocalConnection> &tar) const
{
    NeuronIndex start_src = descr.nid_src0();
    NeuronIndex start_tar = descr.nid_tar0();
    NeuronIndex end_src = descr.nid_src1();
    NeuronIndex end_tar = descr.nid_tar1();

    for (NeuronIndex ind = 0; (start_src+ind) < end_src; ind++) {
        if (shift>=0){
            if (start_tar+ind+shift < end_tar){
                tar.emplace_back(start_src+ind, start_tar+ind+shift, *m_synapse);
            }
            else{
                if (overflow == true)   tar.emplace_back(start_src+ind, start_tar+ind+shift-(end_tar-start_tar), *m_synapse);
                else                    tar.emplace_back(start_src+ind, end_tar-1, *m_synapse);
            }
        }
        else if (shift<0){
            if (end_tar-ind+shift > start_tar){
                tar.emplace_back(end_src-ind-1, end_tar-ind+shift-1, *m_synapse);
            }
            else{
                if (overflow == true)   tar.emplace_back(end_src-ind-1,end_tar-(ind-(end_tar-start_tar))+shift-1,*m_synapse);
                else                    tar.emplace_back(end_src-ind-1,start_tar,*m_synapse);
            }
        }
    }
}

ShiftConnectorMat::ShiftConnectorMat(Real weight, Real delay, signed shift, bool overflow, bool self_connections) : UniformConnector(weight,delay,self_connections)
{
    this->shift_ = shift;
    this->overflow = overflow;
}

void ShiftConnectorMat::connect(const ConnectionDescriptor &descr,std::vector<LocalConnection> &tar) const
{
    NeuronIndex start_src = descr.nid_src0();
    NeuronIndex start_tar = descr.nid_tar0();
    NeuronIndex end_src = descr.nid_src1();
    NeuronIndex end_tar = descr.nid_tar1();
    NeuronIndex diff = end_tar-start_tar;
    NeuronIndex factor = (end_src-start_src)/diff;
    signed shift;
    NeuronIndex start,end;

    for(NeuronIndex row=0; row<factor;row++){
        start = start_src+row*diff;
        shift = shift_*(row+1);
        end = start+diff;
        for (NeuronIndex ind = 0; ind < diff; ind++) {
            if (shift>=0){
                if (start_tar+ind+shift < end_tar){
                    tar.emplace_back(start+ind, start_tar+ind+shift, *m_synapse);
                }
                else{
                    if (overflow == true)   tar.emplace_back(start+ind, start_tar+ind+shift-(end_tar-start_tar), *m_synapse);
                    else                    tar.emplace_back(start+ind, end_tar-1, *m_synapse);
                }
            }
            else if (shift<0){
                if (end_tar-ind+shift > start_tar){
                    tar.emplace_back(end-ind-1, end_tar-ind+shift-1, *m_synapse);
                }
                else{
                    if (overflow == true)   tar.emplace_back(end-ind-1,end_tar-(ind-(end_tar-start_tar))+shift-1,*m_synapse);
                    else                    tar.emplace_back(end-ind-1,start_tar,*m_synapse);
                }
            }
        }
    }
}

CustomConnector::CustomConnector(Real weight, Real delay, std::vector<std::vector<NeuronIndex>> target_list, bool self_connections) : UniformConnector(weight,delay,self_connections)
{
    this->target_list = target_list;
}

OneToVecConnector::OneToVecConnector(Real weight, Real delay, std::string direction, bool self_connections) : UniformConnector(weight,delay,self_connections)
{
    this->direction = direction;
}

void OneToVecConnector::connect(const ConnectionDescriptor &descr, std::vector<LocalConnection> &tar) const
{
    //jedes neuron aus src wird mit einer reihe in tar verbunden
    NeuronIndex start_src = descr.nid_src0();
    NeuronIndex start_tar = descr.nid_tar0();
    NeuronIndex end_src = descr.nid_src1();
    NeuronIndex end_tar = descr.nid_tar1();
    NeuronIndex vectorSize = 0;
    if (direction=="y"){
        vectorSize = (end_tar-start_tar)/(end_src-start_src);
        for(NeuronIndex i = 0; i < (end_src-start_src); i++){
            for(NeuronIndex j = 0; j < vectorSize; j++){
                tar.emplace_back(start_src+i, start_tar+j+i*vectorSize, *m_synapse);
            }
        }
    }
    else if(direction=="x"){
        vectorSize = (end_tar-start_tar)/(end_src-start_src);
        for(NeuronIndex i = 0; i < (end_src-start_src); i++){
            for(NeuronIndex j = 0; j < vectorSize; j++){
                tar.emplace_back(start_src+i, start_tar+i+j*vectorSize, *m_synapse);
            }
        }
    }
}

void CustomConnector::connect(const ConnectionDescriptor &descr, std::vector<LocalConnection> &tar) const
{
    NeuronIndex start_src = descr.nid_src0();
    NeuronIndex start_tar = descr.nid_tar0();
    NeuronIndex end_src = descr.nid_src1();

    for(NeuronIndex i = 0; i < (end_src-start_src); i++){
        for(NeuronIndex j = 0; j < (NeuronIndex)target_list[i].size(); j++){
            tar.emplace_back(start_src+i, start_tar+target_list[i][j], *m_synapse);
        }
    }
}
