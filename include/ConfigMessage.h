#ifndef CONFIGMESSAGE_H
#define CONFIGMESSAGE_H

#include <Message.h>


class ConfigMessage : public Message
{
    public:
        ConfigMessage();
        virtual ~ConfigMessage();
    protected:
    private:
        byte error_number;
        byte event_space_left;
        byte node_variable_index;
        byte node_variable_value;
        byte stored_event_index;
        byte event_store_var_index;
        byte event_store_var;
        byte node_parameter;
        byte node_parameter_index;
        byte number_stored_events;
        byte node_name[7];
        byte node_parameters[7];
        byte stored_event[4];
};

#endif // CONFIGMESSAGE_H
