#pragma once

#include <type_traits>
namespace BeeEngine::Jobs
{
    template <typename T>
    concept CCancelationToken = requires(T t) { requires t.IsCanceled()->bool; };
    template <typename T>
    concept CCancelationTokenProvider =
        requires(T t) { requires BeeEngine::Jobs::CCancelationToken<decltype(t.Provide())>; };
    /*
    CancelationTokenProvider must provide CancelationTokens. And only
    it can cancel Jobs. CancelationTokens have only read access to
    information, whether they are canceled or not.
    For ForEach can be SelfCancelationToken provided.
    */
    class CancelationToken
    {
    }
} // namespace BeeEngine::Jobs