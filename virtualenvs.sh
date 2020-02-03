#!/bin/bash
mkdir ~/venvs
cd ~/venvs/

: "
virtualenv --system-site-packages spinnaker4
source spinnaker4/bin/activate
pip install sPyNNaker8
python -m spynnaker8.setup-pynn
python -c "import spynnaker8"
deactivate

cd ~/venvs/
"
virtualenv --system-site-packages nest
source nest/bin/activate
pip install cython -U
cd nest
wget https://github.com/nest/nest-simulator/archive/v2.14.0.tar.gz
tar -xzvf v2.14.0.tar.gz
cd nest-simulator-2.14.0/ && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=~/venvs/nest/
make -j8 && make install
pip install pyNN==0.9.2 neo==0.5.2 lazyarray
deactivate
: " 
cd ~/venvs/

git clone https://github.com/electronicvisions/spikey_demo.git spikey
cd spikey
./waf setup --project=deb-pynn@0.6
./waf configure
./waf install --targets=*

"
