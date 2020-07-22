#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>


using namespace std;

const double extruder = 0.89;
const double Xoffset = 150.0;
const double Yoffset = 150.0;
const double Zoffset = 0.0;
const double scale = 10.0;


string precisionSetter(double input, int precision){
  stringstream ss;
  string out;
  ss << fixed << setprecision(precision);
  ss << input;
  out = ss.str();
  return out;
}

double distance(double X1,double Y1,double Z1,double X2,double Y2,double Z2){
  return sqrt(pow(X1-X2,2)+pow(Y1-Y2,2)+pow(Z1-Z2,2));
}

vector<double> nozzleWipe(double Xf, double Yf, double Xi, double Yi){
  double norm = distance(Xf,Yf,0.0,Xi,Yi,0.0);
  vector<double> nozzleWipe;
  nozzleWipe.push_back(10*(Xf-Xi)/norm);
  nozzleWipe.push_back(10*(Yf-Yi)/norm);
  return nozzleWipe;
}

vector<vector<double> > parseCSV(string filename)
{
    int i=0,j;
    ifstream  data(filename);
    string line;
    vector<vector<double> > parsedCsv;
    while(getline(data,line))
    {
      i++;
        stringstream lineStream(line);
        string cell;
        double value;
        vector<double> parsedRow;
        while(getline(lineStream,cell,';'))
        {
          if (cell.length()>1 || cell=="0")
          {
            value = stod(cell);
            parsedRow.push_back(value);
          }

        }

        parsedCsv.push_back(parsedRow);
    }

    cout << parsedCsv[0].size() << endl;

    vector<vector<double> > Aux;
    int m = parsedCsv.size()/6;

    for (i=0;i<m;++i)
    {
      Aux.push_back(parsedCsv[i+m]);
      Aux.push_back(parsedCsv[i]);
    }
    for (i=0;i<m;++i)
    {
      Aux.push_back(parsedCsv[3*m+i]);
      Aux.push_back(parsedCsv[2*m+i]);
    }
    for (i=0;i<m;++i)
    {
      Aux.push_back(parsedCsv[5*m+i]);
      Aux.push_back(parsedCsv[4*m+i]);
    }


    return parsedCsv;
}

int main(int argc, char *argv[]){

  vector<vector<double> > toolpath = parseCSV(string(argv[1]));

  ofstream ofs;
  ofs.open(string(argv[2]));


  int L=toolpath.size();

  cout << L << endl;

  int i,j,LL;

  double xWipe, yWipe;

  vector<double> wipe;



  ofs << "G28 ; home all axes" << endl << "G92 X0.0 Y0.0 Z-85 ; reset Zero" << endl << "M201 X500 Y500 E1000 ; set Max Acceleration" <<
    endl << "M202 X500 Y500" << endl << "M203 X200 Y200 Z30 E4000 ; set Max Feedrate" << endl << "M204 P 300 ; set Print Acceleration" <<
    endl << "M204 T 300 ; set Travel Acceleration" << endl << "M205 J0.5 ; set Junction Deviation" << endl << "M205 E5.0 ; set Extruder Jerk" << endl << "M221 S65" << endl;

  ofs << "T0"<< endl;

  for (i=0;i<L/3;++i)
  {
    for (j=0;j<toolpath[i].size();++j)
    {
      toolpath[i][j]=scale*toolpath[i][j]+Xoffset;
      toolpath[i+L/3][j]=scale*toolpath[i+L/3][j]+Yoffset;
      toolpath[i+2*L/3][j]=scale*toolpath[i+2*L/3][j]+Zoffset;
    }
  }

  double layerHeight = ceil(*max_element(toolpath[2*L/3].begin(),toolpath[2*L/3].end()));

  double extValue = 0.0;

  // First layer treated as a special case

  ofs << "; layer 1" << endl << "T0"<< endl << "G92 E0.0000" << endl << "G1 E-0.2000 F10" << endl << "; tool H7.000 W10.000" << endl;

  ofs << "G1 Z"+precisionSetter(toolpath[2*L/3][0]+3,4)+" F300" << endl; // Hovering 5mm over the actual layer height

  ofs << "G1 X"+precisionSetter(toolpath[0][0],4)+" Y"+precisionSetter(toolpath[L/3][0],4)+" Z"+precisionSetter(toolpath[2*L/3][0],4)
    +" F2000" << endl; // Going over to the start point

  ofs << "G1 Z"+precisionSetter(toolpath[2*L/3][0],4)+" F300" << endl; // Descending at right height

  ofs << "G1 E0.0000 F10" << endl << "G92 E0.000" << endl; // Resets extruder to 0

  // First segment set apart to set speed F for all the following segments

  extValue += distance(toolpath[0][0],toolpath[L/3][0],toolpath[2*L/3][0],toolpath[0][1],toolpath[L/3][1],toolpath[2*L/3][1])
    *extruder*toolpath[2*L/3][1]/layerHeight;

  ofs << "G1 X"+precisionSetter(toolpath[0][1],4)+" Y"+precisionSetter(toolpath[L/3][1],4)+" Z"+precisionSetter(toolpath[2*L/3][1],4)+" E"+
    precisionSetter(extValue,4) + " F1000" << endl;

  LL = toolpath[0].size();

  cout << LL << endl;

  for (j=2;j<LL;++j)
  {
    extValue += distance(toolpath[0][j-1],toolpath[L/3][j-1],toolpath[2*L/3][j-1],toolpath[0][j],toolpath[L/3][j],toolpath[2*L/3][j])
      *extruder*(toolpath[2*L/3][j-1]+toolpath[2*L/3][j])/2/layerHeight;

    ofs << "G1 X"+precisionSetter(toolpath[0][j],4)+" Y"+precisionSetter(toolpath[L/3][j],4)+" Z"+precisionSetter(toolpath[2*L/3][j],4)+" E"+
      precisionSetter(extValue,4) +" F1000" << endl;
  }

  // Nozzle wiping


  ofs << "G92 E0.000" << endl << "G1 E-0.2000 F10" << endl;

  xWipe = toolpath[0][LL-1];
  yWipe = toolpath[L/3][LL-1];

  wipe = nozzleWipe(toolpath[0][LL-1],toolpath[L/3][LL-1],toolpath[0][LL-2],toolpath[L/3][LL-2]);

  xWipe += wipe[0];
  yWipe += wipe[1];

  ofs << "G1 X"+precisionSetter(xWipe,4)+" Y"+precisionSetter(yWipe,4) + " F1000" << endl;

  xWipe += -wipe[1];
  yWipe += wipe[0];

  ofs << "G1 X"+precisionSetter(xWipe,4)+" Y"+precisionSetter(yWipe,4) << endl;


  // Layer 2 and on


  for (i=1;i<toolpath.size()/3;++i)
  {
    extValue = 0;

    ofs << "; layer "+to_string(i+1) << endl << "G92 E0.0000" << endl << "G1 E-0.2000 F10" << endl;

    ofs << "G1 Z"+precisionSetter(toolpath[i+2*L/3][0]+5,4)+" F300" << endl; // Hovering 5mm over the actual layer height

    ofs << "G1 X"+precisionSetter(toolpath[i][0],4)+" Y"+precisionSetter(toolpath[i+L/3][0],4)+" Z"+precisionSetter(toolpath[i+2*L/3][0],4)+" F2000" << endl; // Going over to the start point

    ofs << "G1 Z"+precisionSetter(toolpath[i+2*L/3][0],4)+" F300" << endl; // Descending at right height

    ofs << "G1 E0.0000 F10" << endl << "G92 E0.000" << endl; // Resets extruder to 0

    extValue += distance(toolpath[i][0],toolpath[i+L/3][0],toolpath[i+2*L/3][0],
      toolpath[i][1],toolpath[i+L/3][1],toolpath[i+2*L/3][1])*extruder*min(toolpath[i+2*L/3][1]/layerHeight,1.0);

    ofs << "G1 X"+precisionSetter(toolpath[i][1],4)+" Y"+precisionSetter(toolpath[i+L/3][1],4)+" Z"+precisionSetter(toolpath[i+2*L/3][1],4)
      +" E"+precisionSetter(extValue,4) + " F1000"<< endl;

    LL = toolpath[i].size();

    for (j=2;j<toolpath[i].size();++j)
    {
      extValue += distance(toolpath[i][j-1],toolpath[i+L/3][j-1],toolpath[i+2*L/3][j-1],toolpath[i][j],toolpath[i+L/3][j],toolpath[i+2*L/3][j])
        *extruder*min((toolpath[i+2*L/3][j-1]+toolpath[i+2*L/3][j])/2/layerHeight,1.0);

      ofs << "G1 X"+precisionSetter(toolpath[i][j],4)+" Y"+precisionSetter(toolpath[i+L/3][j],4)+" Z"+precisionSetter(toolpath[i+2*L/3][j],4)+
        " E"+precisionSetter(extValue,4) << endl;
    }

    // Nozzle Wiping

    ofs << "G92 E0.000" << endl << "G1 E-0.2000 F10" << endl;

    xWipe = toolpath[i][LL-1];
    yWipe = toolpath[i+L/3][LL-1];

    wipe = nozzleWipe(toolpath[i][LL-1],toolpath[i+L/3][LL-1],toolpath[i][LL-2],toolpath[i+L/3][LL-2]);

    xWipe += wipe[0];
    yWipe += wipe[1];

    ofs << "G1 X"+precisionSetter(xWipe,4)+" Y"+precisionSetter(yWipe,4) + " F1000" << endl;

    xWipe += -wipe[1];
    yWipe += wipe[0];

    ofs << "G1 X"+precisionSetter(xWipe,4)+" Y"+precisionSetter(yWipe,4) << endl;

  }

  ofs << "; layer end" << endl << "M104 S0 ; turn off extruder" << endl << "M140 S0 ; turn off bed" << endl << "M84 ; disable motors" << endl;

}
