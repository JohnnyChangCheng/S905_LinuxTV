#ifndef __DATASTOCK_H__
#define __DATASTOCK_H__
#include <cstring>
#include <mutex>
#include <queue>
#include <thread>
#include <iostream>
namespace Noovo {
	
	template<typename T1> class Datastock {
	public:
		inline Datastock() {
		}
		inline ~Datastock() {
			delete [] _data;
		}
		inline void Set(int size) {
			_size = size;
		}
		inline void Allocate(int size) {
			_data = new T1 [size];
			_size = size;
		}
		inline int Copy(uint8_t* buf, int size){
			int copy=(size > _size)?_size:size;
			memcpy(buf,_data,copy);	
			return copy;
		}
		inline int Returnsize() {
			return _size;
		}
		inline T1* Get() {
			return _data;
		}

	private:
		T1 * _data = nullptr;
		unsigned int _size = 0;
	};
	//For pointer one
	
	template<typename T2> class Queue {
	public:
		Queue(unsigned int max,unsigned int min):_max(max),_min(min){

		}
		~Queue(){

		}
		T2 front_then_pop() {
			T2 front;
			while(1){
				_mutex.lock();
				if(!_queue.empty()){
					front= _queue.front();
					_queue.pop();
					_mutex.unlock();
					return front;
				}else{
					_mutex.unlock();
				}
			}
		}
		void push_check_size(T2 object){
			_mutex.lock();
			_queue.push(object);
			if(_queue.size() >= _max){
				_mutex.unlock();
					while(1){
						_mutex.lock();
						if( _queue.size() < _min){
							_mutex.unlock();
							return;
						}else{
							_mutex.unlock();
							std::this_thread::sleep_for (std::chrono::milliseconds(50));//encoder more slowly so here i sleep
						}
					}
			}else{
				_mutex.unlock();
				return;
			}
		}
		int return_size(){
			_mutex.lock();
			int size  = _queue.size();
			_mutex.unlock();
			return size;
		}

	private:
		std::mutex _mutex;
		std::queue<T2>  _queue;
		unsigned int _max = 0;
		unsigned int _min = 0;
	};
}
#endif