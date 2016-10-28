//
// Created by avpdiver on 10/29/16.
//

#ifndef GLADIUS_SINGLE_THREAD_EXECUTOR_H
#define GLADIUS_SINGLE_THREAD_EXECUTOR_H

namespace gladius { namespace ecs {
        template<typename COMPONENT, typename COMPONENT_STORAGE, typename FUNCTION>
        class c_singlethread_executor {
        public:
            void execute(const COMPONENT_STORAGE& storage, FUNCTION& process) {
                for (auto& component : storage) {
                    process(component);
                }
            }
        };
    }
}

#endif //GLADIUS_SINGLE_THREAD_EXECUTOR_H
