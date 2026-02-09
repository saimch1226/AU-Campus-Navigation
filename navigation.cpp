#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <limits>

using namespace std;

const int INF = numeric_limits<int>::max();

// --- 1. HISTORY MANAGEMENT CLASS ---
struct RouteRecord {
    string from;
    string to;
    int distance;
};

class HistoryManager {
    stack<string> recentSearches; 
    vector<RouteRecord> fullHistory; 

public:
    void addEntry(string startName, string endName, int dist) {
        string entry = startName + " -> " + endName;
        recentSearches.push(entry);
        fullHistory.push_back({startName, endName, dist});
    }

    void showRecent() {
        if (recentSearches.empty()) {
            cout << "\n[!] No recent searches found.\n";
            return;
        }
        cout << "\n--- Recently Visited ---\n";
        stack<string> temp = recentSearches;
        int count = 1;
        while (!temp.empty() && count <= 5) { 
            cout << count++ << ". " << temp.top() << endl;
            temp.pop();
        }
        cout << "------------------------\n";
    }

    void showFullLog() {
        if (fullHistory.empty()) {
            cout << "\n[!] History log is empty.\n";
            return;
        }
        cout << "\n--- Full Route History ---\n";
        for (const auto& r : fullHistory) {
            cout << "Route: " << r.from << " to " << r.to 
                 << " | Distance: " << r.distance << "m\n";
        }
        cout << "--------------------------\n";
    }
};

// --- 2. GRAPH & NAVIGATION CLASS ---
class CampusMap {
    map<int, vector<pair<int, int>>> adj;
    map<int, string> locationNames;
    HistoryManager history; 

public:
    void setLocationName(int id, string name) {
        locationNames[id] = name;
    }

    string getLocationName(int id) {
        if (locationNames.find(id) != locationNames.end()) {
            return locationNames[id];
        }
        return "Path/Turn"; 
    }

    void addEdge(int u, int v, int weight) {
        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight}); 
    }

    // MULTI-ENTRY DEPT HELPER
    void addMultiEntryDept(int masterId, string name, vector<int> entrances, int internalDist = 0) {
        setLocationName(masterId, name);
        for (int ent : entrances) {
            addEdge(masterId, ent, internalDist);
        }
    }

    // DIJKSTRA ALGORITHM
    void findRoute(int start, int end) {
        if (adj.find(start) == adj.end() || adj.find(end) == adj.end()) {
            cout << "\n[Error] Invalid Node ID. Check your map numbers.\n";
            return;
        }

        map<int, int> dist;
        map<int, int> parent;
        for (auto const& [node, neighbors] : adj) {
            dist[node] = INF;
        }
        dist[start] = 0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, start});

        while (!pq.empty()) {
            int currentDist = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (currentDist > dist[u]) continue;
            if (u == end) break; 

            for (auto neighbor : adj[u]) {
                int v = neighbor.first;
                int weight = neighbor.second;

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if (dist[end] == INF) {
            cout << "\n[!] No path found.\n";
            return;
        }

        cout << "\n============================================\n";
        cout << "   ROUTE FOUND: " << dist[end] << " meters\n";
        cout << "============================================\n";

        vector<int> path;
        for (int v = end; v != start; v = parent[v]) {
            path.push_back(v);
        }
        path.push_back(start);
        reverse(path.begin(), path.end());

        for (size_t i = 0; i < path.size(); i++) {
            int id = path[i];
            string name = getLocationName(id);
            int d = 0;

            if (i < path.size() - 1) {
                 for(auto n : adj[id]) if(n.first == path[i+1]) d = n.second;
            }
            
            if (i == path.size() - 2) {
                 cout << " [" << id << "] Arrived at " << getLocationName(path[i+1]) << " Entrance\n";
                 continue;
            }
            if (i == path.size() -1) continue;

            cout << " [" << id << "] ";
            if (name != "Path/Turn") cout << "** " << name << " **";
            else cout << "(Walkway)";
            
            cout << endl;

            if (i < path.size() - 1) {
                cout << "  |\n  V  " << d << "m\n";
            }
        }
        cout << "============================================\n";
        
        // Log to history
        history.addEntry(getLocationName(start), getLocationName(end), dist[end]);
    }

    void viewHistory() {
        history.showRecent();
        history.showFullLog();
    }
};

int main() {
    CampusMap auMap;

    // MAP DATA DEFINITION
    
    // --- 1. DEFINING NAMES ---
    auMap.setLocationName(1, "Main Gate");
    auMap.setLocationName(41, "LTC & Library");
    auMap.setLocationName(49, "Admin Block (Front)"); 
    auMap.setLocationName(20, "Sports Complex");
    auMap.setLocationName(43, "FMC (Medical)");
    auMap.setLocationName(44, "NCSA");
    auMap.setLocationName(45, "B-Block"); 
    auMap.setLocationName(46, "A-Block"); 
    auMap.setLocationName(47, "C-Block (Front)"); 
    auMap.setLocationName(50, "Admin Block (Back)"); 
    auMap.setLocationName(71, "AU Deli");
    auMap.setLocationName(73, "CAFE");
    auMap.setLocationName(74, "Sports Complex (Side)"); 
    auMap.setLocationName(76, "Masjid");
    auMap.setLocationName(77, "FMC Parking");
    auMap.setLocationName(75, "C-Block Parking"); 
    auMap.setLocationName(48, "C-Block (Back)"); 

    // --- 2. DEFINING CONNECTIONS (Edges) ---
    
    // MAIN GATE CONNECTIONS
    auMap.addEdge(1, 2, 20);   // To Node 2 (Path right)
    auMap.addEdge(1, 6, 70);   // To Node 6 (Path left)
    auMap.addEdge(1, 41, 30);  // DIRECT To LTC & Library

    // Path from Node 2
    auMap.addEdge(2, 3, 20);
    auMap.addEdge(3, 4, 60);
    
    // Node 4 Connections
    auMap.addEdge(4, 5, 20);
    auMap.addEdge(4, 71, 40);   // Direct path to AU DELI
    
    // Node 5 & FMC Area
    auMap.addEdge(5, 77, 60);   // Node 5 -> FMC Parking
    auMap.addEdge(77, 43, 60);  // FMC Parking -> FMC Medical College

    // Connect Deli to LTC
    auMap.addEdge(71, 41, 40); 
    
    // Node 13, 14, 71, 77 Connections
    auMap.addEdge(71, 13, 30);  // Deli -> Node 13
    auMap.addEdge(13, 77, 20);  // Node 13 -> FMC Parking
    auMap.addEdge(13, 14, 60);  // Node 13 -> Node 14
    auMap.addEdge(14, 43, 20);  // Node 14 -> FMC

    // FMC (43) & NCSA (44) Connections (Top Path)
    auMap.addEdge(43, 15, 20);  // FMC -> Node 15
    auMap.addEdge(15, 44, 20);  // Node 15 -> NCSA
    auMap.addEdge(44, 16, 25);  // NCSA -> Node 16

    // Path from Node 6 (Left side)
    auMap.addEdge(6, 7, 10); 
    auMap.addEdge(6, 8, 40);
    auMap.addEdge(6, 42, 40); // To IAA East Entrance
    
    // IAA East Connection
    auMap.addEdge(42, 13, 60); // IAA East to Node 13

    // IAA to Admin Area
    auMap.addEdge(8, 72, 40);   // Node 8 -> AU Arena (IAA Backside)
    
    // Node 8, 9, 10, 11 Connections (Bottom Path)
    auMap.addEdge(8, 9, 10);    // Node 8 -> Node 9
    auMap.addEdge(9, 10, 30);   // Node 9 -> Node 10
    auMap.addEdge(10, 17, 60);  // Node 10 -> Node 17 (Path up to Lawns)
    auMap.addEdge(10, 11, 30);  // Node 10 -> Node 11 (Path towards B-Block road)

    // Node 11 Connections
    auMap.addEdge(11, 12, 40);  // Node 11 -> Node 12
    auMap.addEdge(11, 19, 40);  // Node 11 -> Node 19

    // Node 19 Connections
    auMap.addEdge(19, 18, 20);  // Node 19 -> Node 18
    auMap.addEdge(19, 50, 15);  // Node 19 -> Admin Block Lawn

    // Node 72 Connections
    auMap.addEdge(72, 14, 40);  // Connected to top path 14
    auMap.addEdge(72, 24, 5);   // Connected to Node 24

    // Node 24 Connections (The "Hub" near Cafe)
    auMap.addEdge(24, 73, 5);   // Node 24 -> CAFE
    auMap.addEdge(24, 17, 25);  // Node 24 -> Node 17
    
    // Node 17 & 18 Connections (Straight Path Zone)
    auMap.addEdge(17, 18, 30);
    auMap.addEdge(17, 44, 60);  // Node 17 -> NCSA
    
    // Node 18 Connections
    auMap.addEdge(18, 49, 15);  // Node 18 -> Admin Block Front
    auMap.addEdge(18, 16, 50);  // Node 18 -> Node 16

    // Node 16 & B-Block Connections
    auMap.addEdge(16, 45, 15);  // Node 16 -> B-Block
    auMap.addEdge(49, 45, 40);  // Admin Front -> B-Block

    // Node 45 & 46 (A-Block) Connections
    auMap.addEdge(45, 46, 15);  // B-Block -> A-Block
    auMap.addEdge(46, 21, 40);  // A-Block -> Intersection 21
    auMap.addEdge(46, 22, 10);  // A-Block -> Intersection 22

    // Admin / Sports Complex Area
    auMap.addEdge(49, 50, 20); // Walk through/around Admin Block
    
    // Node 50 (Admin Back) Connections
    auMap.addEdge(50, 20, 15); // Admin Back -> Sports Complex

    // Node 20 & 74 (Sports Complex) Connections
    auMap.addEdge(20, 74, 30); // Sports Complex -> Side Entrance 74
    auMap.addEdge(74, 12, 10); // Side Entrance 74 -> Road 12
    
    // C-Block Parking & Back Entrance
    auMap.addEdge(12, 75, 90);  // Node 12 -> C-Block Parking
    
    // Node 48 (Back Ent) is ONLY connected to Parking (75), not Masjid (76)
    auMap.addEdge(75, 48, 20);  // C-Block Parking -> C-Block Back Entrance

    // Node 20 to Node 21 Connection
    auMap.addEdge(20, 21, 20); // Sports Complex -> Intersection 21

    // Left Side (A/B/C Blocks)
    auMap.addEdge(49, 21, 15); // Admin Front -> Intersection 21
    auMap.addEdge(21, 47, 55); // To C-Block Main
    
    // Masjid Connections
    auMap.addEdge(47, 76, 50); // C-Block Front -> Masjid (50m)
    auMap.addEdge(22, 76, 55); // Intersection 22 -> Masjid (55m)

    // --- MULTI-ENTRY SETUP ---
    // 1. IAA Setup (Using New Master ID 33)
    auMap.setLocationName(42, "IAA East Entrance"); 
    auMap.setLocationName(72, "AU Arena (IAA West)");
    auMap.addMultiEntryDept(33, "IAA (Institute of Avionics)", {42, 72}, 30);
    
    // 2. Admin Block Setup
    auMap.addMultiEntryDept(99, "Admin Block (Main)", {49, 50}, 25);

    // 3. C-Block Setup
    auMap.addMultiEntryDept(88, "C-Block", {47, 48}, 25);

    // --- 4. NCSA INTERNAL ROOMS SETUP ---
 
    auMap.setLocationName(101, "NCSA-CR-01");
    auMap.addEdge(44, 101, 30);

    auMap.setLocationName(102, "NCSA-CR-02");
    auMap.addEdge(44, 102, 30);

    auMap.setLocationName(103, "NCSA-CR-03");
    auMap.addEdge(44, 103, 50);

    auMap.setLocationName(104, "NCSA-CR-04");
    auMap.addEdge(44, 104, 60);

    auMap.setLocationName(105, "MAM Memona Office");
    auMap.addEdge(44, 105, 30);

    auMap.setLocationName(106, "NCSA Lab 1");
    auMap.addEdge(44, 106, 20);

    auMap.setLocationName(107, "NCSA Lab 2");
    auMap.addEdge(44, 107, 55);

    auMap.setLocationName(108, "HOD Office");
    auMap.addEdge(44, 108, 20);

    // =========================================================

    int choice;
    do {
        cout << "\n--- AIR UNIVERSITY NAVIGATOR ---\n";
        cout << "1. Find Shortest Route\n";
        cout << "2. View Navigation History\n";
        cout << "0. Exit\n";
        cout << "Select Option: ";
        cin >> choice;

        if (choice == 1) {
            int startNode, endNode;
            cout << "\n ----------------------- COMMON DESTINATIONS -----------------------\n";
            cout << "  [1]  Main Gate           [99] Admin Block (Main)  [33] IAA (Main)\n";
            cout << "  [88] C-Block (Main)      [46] A-Block             [45] B-Block\n";
            cout << "  [44] NCSA                [43] FMC (Medical)       [41] LTC & Library\n";
            cout << "  [76] Masjid              [73] CAFE                [71] AU Deli\n";
            cout << "  [20] Sports Complex      [77] FMC Parking         [75] C-Block Parking\n";
            cout << " -------------------------------------------------------------------\n";
            
            cout << "Enter Start Node ID: ";
            cin >> startNode;
            cout << "Enter Destination Node ID: ";
            cin >> endNode;
        
            if (endNode == 44) {
                cout << "\n   >>> NCSA DEPARTMENT ROOMS <<<\n";
                cout << "   Which room in NCSA are you looking for?\n";
                cout << "   1. NCSA-CR-01 \n";
                cout << "   2. NCSA-CR-02 \n";
                cout << "   3. NCSA-CR-03 \n";
                cout << "   4. NCSA-CR-04 \n";
                cout << "   5. MAM Memona Office \n";
                cout << "   6. Lab 1 \n";
                cout << "   7. Lab 2 \n";
                cout << "   8. HOD Office \n";
                cout << "   0. Just Main Entrance\n";
                cout << "   Enter Choice: ";
                
                int ncsaChoice;
                cin >> ncsaChoice;
 
                switch(ncsaChoice) {
                    case 1: endNode = 101; break;
                    case 2: endNode = 102; break;
                    case 3: endNode = 103; break;
                    case 4: endNode = 104; break;
                    case 5: endNode = 105; break;
                    case 6: endNode = 106; break;
                    case 7: endNode = 107; break;
                    case 8: endNode = 108; break;
                    default: break; 
                }
            }
            // -------------------------------------

            auMap.findRoute(startNode, endNode);
        }
        else if (choice == 2) {
            auMap.viewHistory();
        }

    } while (choice != 0);

    return 0;
}
