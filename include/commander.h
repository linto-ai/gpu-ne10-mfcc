#ifndef COMMANDER_H
#include <iostream>

class Commander{
    public:
    Commander();
    void initPipeline();
    void initBrokerConnect();
    void run();

    private:
    void _on_broker_message();
    void _on_broker_connect();

};

#endif