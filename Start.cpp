#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>

using namespace std;
const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;
const int PARTITION_WIDTH = 200;
const int PARTITION_HEIGHT = 200;


//taken of of internet, well written
unsigned char* createBitmapFileHeader (int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

//taken of of internet, well written
unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}


//vector helper
double magnitude(vector<double> a,vector<double> b){
    if(a.size()!=b.size()){return 0;}
    if(a.size()==0){return 0;}
    if(b.size()==0){return 0;}
    double innerTerm = 0;
    for(int i =0;i<a.size();i++){
        innerTerm += pow(a[i]-b[i],(double)2);
    }
    return sqrt(innerTerm);
}

//vector helper
vector<double> unitVector(vector<double> from,vector<double> to){
    vector<double> uVector;
    double mag =magnitude(from,to);
    for(int i = 0;i<from.size();i++){
        uVector.push_back((to[i]-from[i])/mag);
    }
    return uVector;
}

//vector helper
vector<double> vectorSum(vector<double> a,vector<double> b){
    if(a.size()==0){return b;}
    if(b.size()==0){return a;}
    vector<double> sumVector;
    for(int i=0;i<a.size();i++){
        sumVector.push_back(a[i]+b[i]);
    }
    return sumVector;
}

class simulationObject{
    private:
    double mass;
    vector<double> position;
    vector<double> velocity;
    
    double thickness;

    vector<vector<double>> queuedForces;

    public:
    simulationObject(){};
    simulationObject(double _mass,vector<double> _position,vector<double> _velocity,vector<unsigned char> _color,double _thickness)
    : mass(_mass),position(_position),velocity(_velocity),color(_color),thickness(_thickness){
    }
    vector<unsigned char> color;
    double getMass(){return mass;}
    vector<double> getPosition(){return position;}
    vector<double> getVelocity(){return velocity;}
    void queueForce(vector<double> _force){
        queuedForces.push_back(_force);
    }
    void applyForces(double time){
        vector<double> sForce;
        for(vector<double> thisForce:queuedForces){
            sForce = vectorSum(sForce,thisForce);
        }
        //cout<<"Sforce:"<<to_string(sForce[0])<<":"<<to_string(sForce[1])<<"\n";
        queuedForces.clear();
        vector<double> displacement;
        vector<double> speed;
        for(int i =0;i<sForce.size();i++){
            displacement.push_back(time*velocity[i]+.5*(sForce[i]/mass)*pow(time,2));
           // cout<<i<<" disp"<<displacement[displacement.size()-1]<<"\n";
            speed.push_back(time*(sForce[i]/mass));
            //cout<<"  speed"<<speed[speed.size()-1]<<"\n";
        }
        position=vectorSum(position,displacement);
        velocity=vectorSum(velocity,speed);
        //cout<<"  pos:"<<to_string(position[0])<<":"<<to_string(position[1])<<"\n";
        //cout<<"  vel:"<<to_string(velocity[0])<<":"<<to_string(velocity[1])<<"\n";
    }
};
class simulation{
    private:
    vector<simulationObject> objects;
    vector<simulationObject> backup;
    double gravitationalConstant = 10;
    double gTime;
    //6.674*pow(10,-11)
    public:
    simulation(){

    }

    void addObject(simulationObject _object){
        objects.push_back(_object);
        backup.push_back(_object);
    }

    void reset(){
        objects = backup;
    }

    void step(double time){
        //vector<simulationObject> updatedObjects = objects;
        gTime += time;
        for(int i = 0;i<objects.size();i++){
            for(int j =0;j<objects.size();j++){
                if(i!=j){
                    double gForce = (gravitationalConstant*objects[i].getMass()*objects[j].getMass())/
                                    (magnitude(objects[i].getPosition(),objects[j].getPosition()));
                   // cout<<"gForce:"<<i<<":"<<j<<":"<<to_string(gForce)<<"\n";
                    vector<double> uVec = unitVector(objects[i].getPosition(),objects[j].getPosition());
                   // cout<<"UVec:"<<to_string(uVec[0])<<":"<<to_string(uVec[1])<<"\n";
                    for(int u = 0;u<uVec.size();u++){
                        uVec[u] = uVec[u]*gForce;
                    }
                    //cout<<"gVec:"<<to_string(uVec[0])<<":"<<to_string(uVec[1])<<"\n";
                    objects[i].queueForce(uVec);
                }
            }
        }
        for(int i = 0;i<objects.size();i++){
            objects[i].applyForces(time);
        }
    }


    void snapshot  (unsigned char* data,int partitionStart,int partitionEnd, int _fileX, int _fileY, int _widthInFile, int _heightInFile, int _iterations, double _step, int _snapshotX, int _snapshotY, int _snapshotWidth, int _snapshotHeight, int _xVarDimNum, int _yVarDimNum,int _fileWidth,int _fileHeight)
        
    {
        for(simulationObject o:objects){
            if(o.getPosition()[_xVarDimNum]<=_snapshotX+_snapshotWidth&&o.getPosition()[_xVarDimNum]>=_snapshotX){
            if(o.getPosition()[_yVarDimNum]<=_snapshotY+_snapshotHeight&&o.getPosition()[_yVarDimNum]>=_snapshotY){
                double xRat = ((double)(o.getPosition()[_xVarDimNum]-_snapshotX)/(double)_snapshotWidth);
                double yRat = ((double)(o.getPosition()[_yVarDimNum]-_snapshotY)/(double)_snapshotHeight);
                int xPos = _fileX+(_widthInFile*xRat);
                int yPos = _fileY+(_heightInFile*yRat);
                int fPos = yPos*_fileWidth+xPos;
                if(fPos<=partitionEnd&&fPos>=partitionStart){
                for(int i =0;i<BYTES_PER_PIXEL;i++){
                    unsigned char *tmp = data+(fPos-partitionStart)*BYTES_PER_PIXEL+i;
                    *tmp = o.color[i];
                }
                }
            }
            }
            
        }
    }

    ///Original snapshot code
    /**
    void snapshot(unsigned char* image,int iHeight,int iWidth,int iPixelWidth, double x,double y,double width,double height,int xInd,int yInd){
        for(simulationObject o:objects){
            if(abs(x-o.getPosition()[xInd])>=width/2){continue;}
            int xAdj =iWidth*(o.getPosition()[xInd]-(x-(width/2)))/width;
            if(abs(y-o.getPosition()[yInd])>=height/2){continue;}
            int yAdj =iHeight*(o.getPosition()[yInd]-(x-(height/2)))/height;                                  
            for(int i =0;i<iPixelWidth;i++){
                unsigned char *tmp = image +yAdj*iWidth*iPixelWidth+xAdj*iPixelWidth+i;
                *tmp = o.color[i];
            }
        }
    }**/

    string dataSnapshot(){
        string snap;
        snap =      "___SNAPSHOT____________________\n";
        snap +=     "______TIME: "+to_string(gTime)+"\n";
        for(simulationObject o:objects){
            snap += "______OBJECT___________________\n";
            snap += "_________POSITION______________\n";
            for(double position:o.getPosition()){
            snap += "____________: "+to_string(position)+"\n";
            }
            snap += "_________VELOCITY______________\n";
            for(double velocity:o.getVelocity()){
            snap += "____________: "+to_string(velocity)+"\n";
            }
        }
        return snap;
    }
};



class simulationWrapper{
    private:
    public:
    
    int fileX,fileY,widthInFile,heightInFile;
    int snapshotX,snapshotY,snapshotWidth,snapshotHeight;
    int xVarDimNum,yVarDimNum;
    int fileWidth,fileHeight;
    int iterations;
    double stepVal;
    simulation sim;
    simulationWrapper(simulation _sim, int _fileX, int _fileY, int _widthInFile, int _heightInFile, int _iterations, double _step, int _snapshotX, int _snapshotY, int _snapshotWidth, int _snapshotHeight, int _xVarDimNum, int _yVarDimNum,int _fileWidth,int _fileHeight)
        : sim(_sim), fileX(_fileX), fileY(_fileY), widthInFile(_widthInFile), heightInFile(_heightInFile), iterations(_iterations), stepVal(_step), snapshotX(_snapshotX), snapshotY(_snapshotY), snapshotWidth(_snapshotWidth), snapshotHeight(_snapshotHeight), xVarDimNum(_xVarDimNum), yVarDimNum(_yVarDimNum),fileWidth(_fileWidth),fileHeight(_fileHeight){};

    void snapshot(unsigned char* data,int partitionStart,int partitionEnd){
        sim.snapshot(data,partitionStart,partitionEnd,
        fileX,fileY,widthInFile,heightInFile,
        iterations,stepVal,
        snapshotX,snapshotY,snapshotWidth,snapshotHeight,
        xVarDimNum,yVarDimNum,
        fileWidth,fileHeight
        );
    }
    void step(){
        sim.step(stepVal);
    }
};




class image{

    private:
    vector<simulationWrapper> sims;
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize;
    int widthInBytes;

    public:
    FILE* file;
    int width, height = 0;

    //Constructors//
    image(char* fileName,int _height, int _width):width(_width),height(_height){ 
        file = fopen(fileName, "wb");
        widthInBytes = width * BYTES_PER_PIXEL;
        paddingSize = (4 - (widthInBytes) % 4) % 4;
        
    };
    

    //Substantiation//s
   
    void addSimulation(simulationWrapper sw){
        sims.push_back(sw);
    };


    //Generation//
    void generateImageFile(){
        generateImageMetadata();
        generateImageData();
        fclose(file);
    }

    void generateImageMetadata(){
        int stride = (widthInBytes) + paddingSize;

        unsigned char* fileHeader = createBitmapFileHeader(height, stride);
        fwrite(fileHeader, 1, FILE_HEADER_SIZE, file);

        unsigned char* infoHeader = createBitmapInfoHeader(height, width);
        fwrite(infoHeader, 1, INFO_HEADER_SIZE, file);
    }
    void generateImageData(){
       unsigned char data[PARTITION_WIDTH][PARTITION_HEIGHT][BYTES_PER_PIXEL];
        int imageLength = width*height;
        int pixelsGenerated =0;
        int iNum =0;
        while(pixelsGenerated<imageLength){ 
            
            int partitionStart = pixelsGenerated;
            int partitionLength = (PARTITION_HEIGHT*PARTITION_WIDTH)<imageLength-partitionStart?(PARTITION_HEIGHT*PARTITION_WIDTH):imageLength-partitionStart;
            int partitionEnd = partitionStart+partitionLength;

            int color[3] = {0,0,0};
            //Generate Data for current partition
            for (int i = 0; i < PARTITION_WIDTH; i++) {
                for (int j = 0; j < PARTITION_HEIGHT; j++) {
                    data[i][j][2] = (unsigned char)color[2];///red
                    data[i][j][1] = (unsigned char)color[1];///green
                    data[i][j][0] = (unsigned char)color[0];///blue
                }  
            }
cout<<"PS:"<<partitionStart<<"pE:"<<partitionEnd<<"PL:"<<partitionLength<<"\n";
            for(simulationWrapper sw : sims){
                sw.sim.reset();
                for(int i=0;i<sw.iterations;i++){
                
                sw.snapshot((unsigned char*)data,partitionStart,partitionEnd);
                sw.step();
                }
            }

            int datapushed=0;
            while(datapushed<partitionLength){
                int currentPush = width-((partitionStart+datapushed)%width)>partitionLength-datapushed?partitionLength-datapushed:width-((partitionStart+datapushed)%width);
                fwrite((unsigned char*)data+datapushed*BYTES_PER_PIXEL,BYTES_PER_PIXEL,currentPush,file);datapushed += currentPush;
                if(datapushed<partitionLength){
                    fwrite(padding, 1, paddingSize, file);
                }
            }
            iNum +=25;
            pixelsGenerated += partitionLength;
            cout<<"PixGen:"<<pixelsGenerated<<"ImLength"<<imageLength<<"PEnd"<<partitionEnd<<"pStart"<<partitionStart<<"\n";
        }
    }






   

    
};
//other



/**
void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}
**/



int main ()
{
    int IMAGE_HEIGHT =400;
    int IMAGE_WIDTH =400;
    image myImage = image((char*)"MyImageFile.bmp",IMAGE_WIDTH,IMAGE_HEIGHT);
    simulation mySim;
    mySim.addObject(simulationObject(100,{125,100,100},{2,0,-5},{0,0,125},5));
    mySim.addObject(simulationObject(100,{100,125,100},{1,-5,0},{0,125,0},5));
    mySim.addObject(simulationObject(100,{100,100,125},{-5,3,0},{125,0,0},5));
    simulationWrapper sw = simulationWrapper(mySim,0,0,200,200,5000,.001,70,70,60,60,0,1,IMAGE_WIDTH,IMAGE_HEIGHT);
    simulationWrapper sw2 = simulationWrapper(mySim,200,0,200,200,5000,.001,70,70,60,60,0,2,IMAGE_WIDTH,IMAGE_HEIGHT);
    
    myImage.addSimulation(sw);
    myImage.addSimulation(sw2);
    myImage.generateImageFile();


    /**
    simulation mySimulation;
    
    
    //mySimulation.addObject(simulationObject((1988500*pow(10,24)),{0,0,0},{0,0,0},{000,255,000},3));
    //mySimulation.addObject(simulationObject((5.9724*pow(10,24)),{150000000*pow(10,3),0,0},{0,8000000000,0},{255,000,000},3));
    //mySimulation.addObject(simulationObject(dub,{(150000000*pow(10,3)+378000*pow(10,4)),0,0},{-10000000,8500000000,0},{000,000,255},3));
    mySimulation.addObject(simulationObject(100,{10,0,0},{2,3,0},{125,000,125},3));
    mySimulation.addObject(simulationObject(100,{0,10,0},{1,-3,0},{000,125,125},3));
   // mySimulation.addObject(simulationObject(100,{0,0,0},{-10,0,10},{000,000,125},3));
   // mySimulation.addObject(simulationObject(100,{4,4,4},{-1,-1,-1},{125,000,000},3));
    //a = mother/r^2

    const int imgHeight = 512;
    const int imgWidth = 512;
    const int imgPixelWidth = 3;
    unsigned char xyView[imgHeight][imgWidth][imgPixelWidth];
    char* xyFileName = (char*) "XY.bmp";
    for (int i = 0; i < imgHeight; i++) {
        for (int j = 0; j < imgWidth; j++) {
            xyView[i][j][2] = (unsigned char) (000);///red
            xyView[i][j][1] = (unsigned char) (000);///green
            xyView[i][j][0] = (unsigned char) (000);///blue
        }
    }
    unsigned char xzView[200][200][imgPixelWidth];
    char* xzFileName = (char*) "XZ.bmp";
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            xzView[i][j][2] = (unsigned char) (000);///red
            xzView[i][j][1] = (unsigned char) (000);///green
            xzView[i][j][0] = (unsigned char) (000);///blue
        }
    }
    int iterations = 100000;
    double step = .001;
    for(int i = 0;i<iterations;i++){
        mySimulation.step(step);
        if(i%(iterations/10)==0){cout<<i/(iterations/10);};
        //cout<<mySimulation.dataSnapshot()<<"\n";
        mySimulation.snapshot((unsigned char*) xyView,imgHeight,imgWidth,imgPixelWidth,0,0,50,50,0,1);
        mySimulation.snapshot((unsigned char*) xzView,200,200,imgPixelWidth,0,0,50,50,0,1);
    }

    generateBitmapImage((unsigned char*) xyView, imgHeight, imgWidth, xyFileName);
    generateBitmapImage((unsigned char*) xzView, 200, 200, xzFileName);
    printf("Image generated!!");
    **/
}