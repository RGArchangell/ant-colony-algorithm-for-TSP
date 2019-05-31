#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#define streams freopen("input.txt", "r", stdin); freopen("output.txt", "w", stdout);
using namespace std;

int graphSize;
double alpha = 1.6;
double beta = 1.0;
double pheromoneEvac = 0.5;
double pheromones[30][30];

struct Point
{
    double x;
    double y;
};

vector<Point> dataA;
void fillGraph()
{
    for (int i = 0; i < graphSize; i++)
    {
        Point a;
        cin >> a.x >> a.y;
        dataA.push_back(a);
    }
}

double countDist(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

class Ant
{
private:
    int startCity;
    int curSity;
    int curStep;
    
    double wayLength = 0.0;
    double pheromonePerLay = 0.5;
    
    vector<int> memory;
    vector<double> attract;
    
    bool checkEnding()
    {
        if (curStep == graphSize)
            return true;
        else
            return false;
    }
    
    bool check(vector<int> memory, int city)
    {
        for (int i = 0; i < graphSize; i++)
        {
            if (city == memory[i])
                return false;
        }
        return true;
    }
    
public:
    double pheromoneThisWay = 0.0;
    
    void initAnt(int num)
    {
        vector<int> n1(30, -1);
        memory = n1;
        vector<double> n2(30, -1.0);
        attract = n2;
        
        wayLength = 0.0;
        
        startCity = num;
        curSity = startCity;
        curStep = 1;
        memory[0] = startCity;
    }
    
    void layPheromone(int from, int to)
    {
        if (wayLength > 0)
        {
            pheromoneThisWay = pheromonePerLay / wayLength;
            pheromones[from][to] += pheromoneThisWay;
            pheromones[to][from] += pheromoneThisWay;
        }
        else
        {
            pheromones[from][to] += pheromonePerLay;
            pheromones[to][from] += pheromonePerLay;
        }
    }
    
    void nextStep()
    {
        if (checkEnding())
        {
            wayLength += countDist(dataA[curSity].x, dataA[curSity].y, dataA[startCity].x, dataA[startCity].y);
            //cout << "ANT NO WAY HAS ENDED" << endl;
        }
        else
        {
            double sum = 0.0;
            for (int j = 0; j < graphSize; j++)
            {
                if (check(memory, j))
                {
                    double vision1 = 1 / countDist(dataA[curSity].x, dataA[curSity].y, dataA[j].x, dataA[j].y);
                    sum += (pow(pheromones[curSity][j], alpha) + pow(vision1, beta));
                }
            }
            for (int i = 0; i < graphSize; i++)
            {
                
                double vision = 1 / countDist(dataA[curSity].x, dataA[curSity].y, dataA[i].x, dataA[i].y);
                double upper = (pow(pheromones[curSity][i], alpha) + pow(vision, beta));
                attract[i] = upper / sum;
                
                if (!check(memory, i))
                    attract[i] = 0;
            }
            
            while (memory[curStep] == -1)
            {
                double WTF = ((double)rand() / (RAND_MAX));
                for (int i = 0; i < graphSize; i++)
                {
                    //cout << WTF << " " << attract[i] << endl;
                    if (WTF <= attract[i])
                    {
                        layPheromone(curSity, i);
                        wayLength += countDist(dataA[curSity].x, dataA[curSity].y, dataA[i].x, dataA[i].y);
                        
                        curSity = i;
                        memory[curStep] = i;
                        
                        break;
                    }
                }
                //cout << endl;
            }
            
            curStep++;
        }
    }
    
    void debugAnt()
    {
        cout << "Start city: " << startCity << endl;
        cout << "Current city: " << curSity << endl;
        cout << "Route length: " << wayLength << endl;
        cout << "Step number: " << curStep << endl;
        cout << "I have visited: ";
        for (int i = 0; i < graphSize; i++)
        {
            cout << memory[i] << " ";
        }
        cout << endl;
    }
    
    double showWayLength()
    {
        return wayLength;
    }
    
    vector<int> showAntRoute()
    {
        vector<int> answer;
        int i = 0;
        while (memory[i] >= 0)
        {
            answer.push_back(memory[i]);
            i++;
        }
        
        return answer;
    }
};

class Swarm
{
public:
    vector<Ant> ants;
    double bestWayLen = INFINITY;
    vector<int> bestRoute;
    double pheromonePerWholeSwarm = 0.0;
    
    void init()
    {
        for (int i = 0; i < graphSize; i++)
        {
            for (int j = 0; j < graphSize; j++)
            {
                if (i != j)
                    pheromones[i][j] = 1.0;
                else
                    pheromones[i][j] = 0.0;
            }
        }
        
        initAnts();
    }
    
    void initAnts()
    {
        for (int i = 0; i < graphSize; i++)
        {
            Ant newAnt;
            newAnt.initAnt(i);
            ants.push_back(newAnt);
        }
    }
    
    void runSwarm()
    {
        for (int turn = 0; turn < 35; turn++)
        {
            for (int s = 0; s < graphSize; s++)
            {
                for (int i = 0; i < graphSize; i++)
                {
                    ants[i].nextStep();
                    pheromonePerWholeSwarm += ants[i].pheromoneThisWay;
                }
                pheromoneEvacuate();
                pheromonePerWholeSwarm = 0.0;
            }
            
            updateBestRoute();
            ants.clear();
            initAnts();
        }
        
    }
    
    void pheromoneEvacuate()
    {
        for (int i = 0; i < graphSize; i++)
            for (int j = 0; j < graphSize; j++)
            {
                pheromones[i][j] = ((1 - pheromoneEvac) * pheromones[i][j] + pheromonePerWholeSwarm);
                if (pheromones[i][j] < 0)
                {
                    pheromones[i][j] = 0;
                }
            }
    }
    
    void updateBestRoute()
    {
        for (int i = 0; i < graphSize; i++)
            if (ants[i].showWayLength() < bestWayLen)
            {
                bestWayLen = ants[i].showWayLength();
                bestRoute = ants[i].showAntRoute();
            }
    }
    
    void returnSolution()
    {
        cout << setprecision(10) << bestWayLen << endl;
        for (int i = 0; i < graphSize; i++)
            cout << bestRoute[i] + 1 << " ";
    }
    
    void showSwarm()
    {
        for (int i = 0; i < graphSize; i++)
        {
            cout << "ANT NO " << i << ": " << endl;
            ants[i].debugAnt();
            cout << endl << endl;
        }
    }
};

int main()
{
#ifndef LOCAL
    streams;
#endif
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin >> graphSize;
    fillGraph();
    
    Swarm swarm;
    swarm.init();
    
    swarm.runSwarm();
    swarm.returnSolution();
    
    return 0;
}
