// ifstream constructor.
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <vector> 
#include <list> 
#include <thread> 
#include <random>
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <sstream>

using namespace std;

/*
 * This is the struct for each hiker. They have attribute, name and speed.
 */
struct hiker
{
	string name;
	float speed;
public:
	hiker(string name, float speed)
        : name(name), speed(speed)
     	{
   	}
	void operator = (const hiker &hkr) {
        	name = hkr.name;
        	speed = hkr.speed;
        }
};


/*
 * This is the class for calculating the result of the trip
 */
class result_of_hiking_trip
{
	vector<float> result_of_each;
	std::mutex m_result_lock;

public:
	result_of_hiking_trip()
	{
	}

	void store_in_result_array(float val)
	{
		result_of_each.push_back(val);
	}

	vector<float> get_result_array()
	{
		return result_of_each;
	}

	void result_lock()
	{
		m_result_lock.lock();
	}

	void result_unlock()
	{
		m_result_lock.unlock();
	}

	float calculate_result_array()
	{
		float total_resultant_time = 0;

		for(int i = 0; i < result_of_each.size(); i++)
		{
			total_resultant_time = result_of_each[i] + total_resultant_time;
		}
	
		return total_resultant_time;
	}
};


/*
 * Only one object is enough for the whole trip. 
 */
result_of_hiking_trip single_result_object;


/*
 * This is the main list which gets populated by the Master Node. Then parts of this list will be sent to each worker nodes for processing
 */
list<struct hiker> *hlist;


/*
 * This is a function called from the Master Node. It creates lists for job submission which will be eventually sent to the worker nodes.
 */
vector<struct hiker> create_list_for_job(list<struct hiker> *hlist, int job)
{
	list<struct hiker>::iterator it;
	vector<struct hiker> h;

	for(int i = 0;i <= job; i++)
	{
    		for (it = hlist[i].begin(); it != hlist[i].end(); ++it)
		{
			struct hiker hk("",0);
			
			hk.name = it->name;
			hk.speed = it->speed;
			h.push_back(hk);

		}
	}

	return h;

}

/*
 * This is for the priority queue.
 */
struct compare_speed {
    bool operator()(hiker const& p1, hiker const& p2)
    {
        // return "true" if "p1" is ordered
        // before "p2", for example:
        return p1.speed < p2.speed;
    }
};


/*
 * The functions of these class are executed at each Worker nodes.
 */
class calculate_fastest_time_each_bridge
{
	vector<struct hiker> hikers;
	queue<struct hiker> qh;
    	priority_queue<struct hiker, vector<struct hiker>, compare_speed> pq;
	int total_hikers;
	int bridge_length;
	int bridge_no;

public:
	calculate_fastest_time_each_bridge(vector<struct hiker> hkr, int bridge_no, int total_hikers, float bridge_length)
	{
		for(int i = 0;i < hkr.size(); i++)
		{
			struct hiker h("",0);
			h.name = hkr[i].name;
			h.speed = hkr[i].speed;
			this->hikers.push_back(h);
		}
		this->bridge_length = bridge_length;
		this->bridge_no = bridge_no;
		this->total_hikers = hkr.size();
#if 0
		build_pq();
		push_hikers_in_queue();
#endif
	}

	void build_pq()
	{
		for(int i = 0; i < hikers.size(); i++)
		{
			pq.push(hiker(hikers[i].name,hikers[i].speed));
		}
	}

	vector<struct hiker> get_hikers()
	{
		return hikers;
	}

	struct hiker calculate_torch_guy()
	{
		struct hiker hkr("",0);
		hkr.name = pq.top().name;
		hkr.speed = pq.top().speed;

		return hkr;
	}

	void push_hikers_in_queue()
	{
		struct hiker hkr("",0); 
		hkr = calculate_torch_guy();
		for(int i = 0;i < hikers.size(); i++)
		{
			if(hkr.name != hikers[i].name)
			{
				qh.push(hikers[i]);
			}
		}
	}
	int get_no_of_hikers()
	{
		return total_hikers;
	}

	float get_length_of_bridge()
	{
		return bridge_length;
	}

	int cal_time_to_cross_the_bridge()
	{
		float fastest_time_to_cross_this_bridge = 0;
		struct hiker hkr("", 0); 
		hkr = calculate_torch_guy();
		float speed_of_torch_guy = hkr.speed;
		int no_of_hikers = get_no_of_hikers(); 
		int total_pair_trip = no_of_hikers - 1;
		int total_single_trip = total_pair_trip -1;
		float time_for_pair_trip = 0;
		float len_of_bridge = get_length_of_bridge();
		float time_for_single_trip = len_of_bridge / speed_of_torch_guy;
		float total_time_for_single_trip = time_for_single_trip * total_single_trip;
		float total_time_for_pair_trip = 0;

		// Torch guy escorts all the remaining guys
		while(!qh.empty()) {
			float speed = qh.front().speed;
			string name = qh.front().name;

			total_time_for_pair_trip = total_time_for_pair_trip + len_of_bridge / speed; 	
			qh.pop();
		}

		fastest_time_to_cross_this_bridge = total_time_for_single_trip + total_time_for_pair_trip; 

		return fastest_time_to_cross_this_bridge;
	}
};

/*
 * Master node function to create a list of the bridge length.
 */
int get_list_of_bridge_len(vector<float>& vec_blen)
{
	string next;
	ifstream fin;

	fin.open("hiking.yaml", ios::in);
	if(!fin)
	{
		cout<<"Error in opening the file"<<endl;
		return -1;
	}

	while(getline(fin,next))
	{
		int found = next.find("bridge");
		if(found != std::string::npos)
		{
			string sstr_blen = next.substr(next.find(":") + 1);
			std::string::size_type sz;   // alias of size_t
			int blen = stoi(sstr_blen, &sz);
			vec_blen.push_back(blen);
		}
	}

	return 0;
}


/*
 * This thread calls the functions of the class-> calculate_fastest_time_each_bridge at each worker node.
 */
void thread_task(calculate_fastest_time_each_bridge& o) {
	vector<struct hiker> h;

	h = o.get_hikers();

	for(int i = 0; i < h.size(); i++)
	{
		struct hiker hkr("",0);
		hkr.name = h[i].name;
		hkr.speed = h[i].speed;
	}
	o.build_pq();
	o.push_hikers_in_queue();
	float each_time = o.cal_time_to_cross_the_bridge();
	ofstream result_file;


	result_file.open("result.out", ios::out | ios::app);
	if(!result_file)
	{
		cout<<"Error in opening the file result.out"<<endl;
	}
	else
	{
  		result_file << to_string(each_time);
  		result_file << " ";
  		result_file.close();
	}

	single_result_object.result_lock();
	single_result_object.store_in_result_array(each_time);
	single_result_object.result_unlock();
}

/*
 *  Master node: Parsing the YAML file, book keeping, and creating jobs for submission to the worker nodes. It collects results from worker nodes and prints.
 */

int main(int argc, char** argv) {

	string next;
	int job = 0;
	int nflag = 0;
	int sflag = 0;
	string n;
	float s;
	ifstream fin;
	vector<float> vec_blen;
	int total_bridges = 0;

	if(argc < 2 || argc > 2)
	{
		std::cerr << "Usage: " << argv[0] << "  <filename.yaml>" << std::endl;
		return -1;
	}

	fin.open(argv[1], ios::in);
	if(!fin)
	{
		cout<<"Error in opening the file"<<endl;
		return -1;
	}

	if(get_list_of_bridge_len(vec_blen) != -1)
	{
		total_bridges = vec_blen.size();
	}
	hlist = new list<struct hiker>[total_bridges];
	if(!hlist)
	{
		cout<<"Memory Error in allocation"<<endl;
		return -1;
	}

	while(!fin.fail())
	{
		while(getline(fin,next))
		{
			int found = next.find("hikers");
			if(found != std::string::npos)
			{
				while(getline(fin,next))
				{
					int found1 = next.find("hikers");
					int found2 = next.find("bridge");
					if(found1 == std::string::npos && found2 == std::string::npos)
					{
						int f1 = next.find("name");
						if(f1 != string::npos)
						{
							string sp = next.substr(next.find(":") + 1);
							n = sp;
							nflag = 1;
						}
						int f2 = next.find("speed");
						if(f2 != string::npos) {
							string sp = next.substr(next.find(":") + 1);
							std::string::size_type sz;   // alias of size_t
							s = stof(sp,&sz);
							sflag = 1;

						}
						if(nflag && sflag) {
							struct hiker hkr("",0);
							hkr.name = n;
							hkr.speed = s;
							hlist[job].push_back(hkr);
							nflag = 0;
							sflag = 0;
						}
					}
					else
					{
						job++;
						break;
					}
				
				}
			}
		}

		fin.close();
	}

	for(int i = 0; i <= job; i++)
	{
		vector<struct hiker> h;
		vector<float> vec_blen;

		h = create_list_for_job(hlist,i);
		if(get_list_of_bridge_len(vec_blen) == -1) {
			if(hlist)
				delete hlist;
			return -1;
		}

		// Create threads for consurrency
		calculate_fastest_time_each_bridge obj(h, i, h.size(), vec_blen[i]);
        	std::thread t1(thread_task, std::ref(obj));
        	t1.join();
	}
	single_result_object.result_lock();
	float result = single_result_object.calculate_result_array();
	cout<<"Total fastest Time for the trip by using shared memory method = "<< " "<< result << " " <<"minutes"<< endl;
	single_result_object.result_unlock();


	std::ifstream ifs_result("result.out", std::ifstream::in);
	string result_str;
	string tok("");
	while(getline(ifs_result,result_str))
		;
	vector <int> result_array;
	float total_fastest_time_result = 0;

	stringstream ss(result_str);


	string intermediate;

	// Tokenizing with respect to ' '
	while(getline(ss, intermediate, ' '))
	{
		std::string::size_type sz;   // alias of size_t
		s = stof(intermediate,&sz);
		result_array.push_back(s);
	}

	for(int i = 0; i < result_array.size(); i++)
	{
		total_fastest_time_result = total_fastest_time_result + result_array[i];
	}

	cout<<"Total fastest Time for the trip by using file method = "<< " "<< total_fastest_time_result << " " <<"minutes"<< endl;


	ifs_result.close();
	std::remove("result.out");

	return 0;

}
