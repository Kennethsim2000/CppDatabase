#pragma once

#include "types.h"

namespace db
{
    class Column
    {
    public:
        Column(
            std::string name,
            TypeId type)
            : name_(std::move(name)),
              type_(type)
        {
        }

        const std::string &name() const
        {
            return name_;
        }

        TypeId type() const
        {
            return type_;
        }

    private:
        std::string name_;
        TypeId type_;
    };
}