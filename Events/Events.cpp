// evh_native.cpp
#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include <functional>
#include <iostream>


class Event {
private:
	//List of subscribers to this event
	mutable std::vector<std::function<void()>> subscribers;

public:
	//Invoke all methods inside this Event
	void Invoke() const {
		for (int i = 0; i < subscribers.size(); i++)
			if(subscribers[i] != nullptr)
				subscribers[i]();
	}
	//Add a new subscriber to this Event
	int Add(const std::function<void()> message) const {
		int subscriptionIndex = -1;

		for (int i = 0; i < subscribers.size() && subscriptionIndex == -1; i++)
			if (subscribers[i] == nullptr)
				subscriptionIndex = i;

		if (subscriptionIndex == -1) {
			subscriptionIndex = (int)subscribers.size();
			subscribers.push_back(message);
		}
		else
			subscribers[subscriptionIndex] = message;
		return subscriptionIndex;
	}
	//Remove a subscriber from this Event
	void Remove(const int subscriptionIndex) const {
		subscribers[subscriptionIndex] = nullptr;
	}
};

//Raii watcher set up
class Watch {
private:
	//Event this watch is subscribed to
	Event& subscribedEvent;

	//The index this subscription refers to
	int subscriptionIndex;
public:
	//Hook watcher
	Watch(Event& subscribedEvent, std::function<void()> message) :
		subscribedEvent(subscribedEvent){
		subscriptionIndex = subscribedEvent.Add(message);
		std::cout << "Subscribed " << subscriptionIndex << std::endl;
	}
	//Unhook watcher
	~Watch() {
		subscribedEvent.Remove(subscriptionIndex);
		std::cout << "Unsubscribed " << subscriptionIndex << std::endl;
	}
};


///-----EXAMPLE-----

//Subject
//Generic engine class
class Engine {
public:
	//On Update Event
	Event onUpdate;

	//On Update message call
	void Update() {
		onUpdate.Invoke();
	}
};


//Observer
class Widget {
	//Engine reference
	Engine& engine;
	
	//Local on update message
	void OnUpdate() {
		std::cout << "Updated" << std::endl;
	}

	//Watcher set up
	Watch onUpdate = Watch(engine.onUpdate, std::bind( &Widget::OnUpdate, this ));

	//Constructor
public:
	Widget(Engine& e) : engine(e) { }
};




int main() {
	Engine e;
	//test widgets
	Widget us1(e);
	{
		Widget us2(e);
		Widget us3(e);
	}
	Widget us4(e);

	e.Update();
	return 0;
}