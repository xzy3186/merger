//
//  BRIKENTypes.h
//  event
//
//  Created by Jorge Agramunt Ros on 03/10/16.
//
//

#ifndef BRIKENTypes_h
#define BRIKENTypes_h

#include <mutex>
#include <iostream>
typedef uint32_t mainMapSecond;

typedef int64_t TSTime_t;
typedef int64_t TSTimeWindow_t;
typedef uint8_t ShortIndex_t;
typedef uint16_t LongIndex_t;
typedef uint8_t DetectorType_t;
typedef  uint16_t Cycles_t;

//bool CheckCycle=false;



class MapKey{
public:
//    static bool CheckCycle;
    bool CheckCycle=false;

    TSTime_t TS;
    Cycles_t Cycle;
    
    MapKey(){
        TS=0;
        Cycle=0;
    };
    MapKey(const MapKey  &NewKey){
        Cycle=NewKey.Cycle;
        TS=NewKey.TS;
    };
    MapKey(Cycles_t cycle,TSTime_t ts ):TS(ts),Cycle(cycle){};
    
    ~ MapKey(){};
    
     void Clear(){
        TS=0;
        Cycle=0;
    };
//    void operator=(const  Cycles_t cycle,TSTime_t ts)const {
//        Cycle=cycle;
//        TS=ts;
//    };
    
    bool operator>(const  MapKey &Input)const {
        if(CheckCycle){
        if(Cycle==Input.Cycle)return(Input.TS<TS);
        else return(Input.Cycle < Cycle);
        }
        else return(Input.TS<TS);
    }
    bool operator>=(const  MapKey &Input)const {
        if(CheckCycle){
        if(Cycle==Input.Cycle)return(Input.TS<=TS);
        else return(Input.Cycle < Cycle);
        }
        else        return(Input.TS<=TS);
    }
    bool operator<(const  MapKey &Input)const {
        if(CheckCycle){
        if(Cycle==Input.Cycle)return(Input.TS>TS);
        else return(Input.Cycle > Cycle);
        }
        else        return(Input.TS>TS);
    }
    bool operator<=(const  MapKey &Input)const {
        if(CheckCycle){
        if(Cycle==Input.Cycle)return(Input.TS>=TS);
        else return(Input.Cycle > Cycle);
        }
        else        return(Input.TS>=TS);
    }
    bool operator==(const  MapKey &Input)const {
        if(CheckCycle){
            return(Input.Cycle == Cycle && Input.TS==TS);
        }
        else return(Input.TS==TS);
    }
    
    MapKey AddTS(TSTime_t T) const {
        MapKey Dummy;
        Dummy.TS=this->TS+T;
        Dummy.Cycle=this->Cycle;
        return Dummy;
        
    }
};

//bool MapKey::CheckCycle=false;

typedef  MapKey MapKey_t;

template <typename K, typename V>
class Bmap : public std::multimap<K,V>
{
    //Constructor
public:
    void lock(){_MyMutex.lock();};
    void unlock(){_MyMutex.unlock();};
    MapKey_t NewTS;
    
    void SetNewest(const MapKey_t  & New){
        NewTS=New;
    };
    MapKey_t GetNewest( ){
        return NewTS;
    };
    
    Bmap() : std::multimap<K, V>() {};
    void Emplace(K const &k,V const &v){
        std::cout<<"kkkkk"<<std::endl;
        this->emplace(k,v);
        
    }
    std::pair<K,V> last(){
        std::pair<K,V> r;
        _MyMutex.lock();
    
        if(this->size()>0){
            auto endIt=this->end();
            --endIt;
            _MyMutex.unlock();
            return  *(endIt);
            
        }
        _MyMutex.unlock();
        
    }
    std::pair<K,V> first(){
        _MyMutex.lock();
        std::pair<K,V> r= *(this->begin());
        _MyMutex.unlock();
        return r;
    }
    bool is_uper_bound(K const k){
        
        _MyMutex.lock();
        bool r=this->upper_bound(k)==this->end();
        _MyMutex.unlock();
        return r;
        
    }
    void berase(){
        _MyMutex.lock();
        this->erase(this->begin());
        _MyMutex.unlock();
        
    }
    int Count(K const k){
        _MyMutex.lock();
        int r=this->count(k);
        _MyMutex.unlock();
        return r;
        
    }
    
    //The get method
    V get(K key)
    {
        typename std::map<K, V>::const_iterator iter(find(key));
        return iter != this->end() ? iter->second : V();
    }
protected:
    
    std::mutex _MyMutex;
    
};

enum DataTypes {Neu=1, Gamma=2, Anc=3, Imp=4, Beta=5, Pid=6};

#endif /* BRIKENTypes_h */
