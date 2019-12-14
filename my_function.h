#pragma once

template <typename T>
class my_function;

template <typename RET, typename ... PARAMS>
class my_function<RET (PARAMS...)>
{
    struct callable_base
    {
        virtual ~callable_base() = default;
        virtual RET operator()(PARAMS...) = 0;
    };

    template <typename FunctionType>
    struct callable : public callable_base
    {
        FunctionType f;

        explicit callable(FunctionType f) : f(f) {}

        RET operator()(PARAMS... params) final
        {
            return f(std::forward<PARAMS>(params)...);
        }
    };

    std::unique_ptr<callable_base> func;

public:
    my_function() = default;    // func is nullptr by default
    ~my_function() = default;

    template <typename FunctionType>    // lambda, functor etc.
    constexpr my_function(FunctionType f) : func(std::make_unique<callable<FunctionType>>(callable<FunctionType>(f)))
    {
        // filtering nullptr function ptrs
        if constexpr (std::is_pointer_v<FunctionType>)
        {
            // It has a minuscule runtime cost if we don't want to duplicate code
            if (f == nullptr)
            {
                func.reset(nullptr);
            }
        }
    }

    // attempted initialization with nullptr
    constexpr my_function(std::nullptr_t)
    {
        func.reset(nullptr);
    }

    constexpr my_function(const my_function& other) : func(new callable<decltype(*other.func)>(*other.func))
    {

    }

    constexpr my_function(my_function&& other) noexcept = default;

    // copy and swap technique to ensure strong exception safety
    // (copy assign and move assign grouped together)
    constexpr my_function& operator=(my_function other)
    {
        std::swap(other.func, func);
        return *this;
    }

    RET operator()(PARAMS... params)
    {
        if (func == nullptr)
        {
            throw std::bad_function_call();
        }

        return (*func)(std::forward<PARAMS>(params)...);
    }

    explicit operator bool()
    {
        // unique_ptr bool operátora kell
        return static_cast<bool>(func);
    }
};