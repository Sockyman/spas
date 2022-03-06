#include "context.h"
#include "node.h"

int get_address(Context *context)
{
    if (context->region == SECTION_DATA)
    {
        return context->address_data;
    }
    return context->address_text;
}

void set_address(Context *context, int value)
{
    if (context->region == SECTION_DATA)
    {
        context->address_data = value;
    }
    else
    {
        context->address_text = value;
    }

}

