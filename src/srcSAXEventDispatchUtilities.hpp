#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include <initializer_list>


#ifndef INCLUDED_SRCSAX_EVENT_DISPATCH_UTILITIES_HPP
#define INCLUDED_SRCSAX_EVENT_DISPATCH_UTILITIES_HPP

namespace srcSAXEventDispatch{
    template <typename ...policies>
    class srcSAXEventDispatcher;            
    enum ElementState {open, close};
    enum ParserState {decl, expr, parameter, declstmt, exprstmt, parameterlist, 
        argumentlist, argumentlisttemplate, call, templates, ctrlflow, endflow, genericargumentlist,
        name, function, functiondecl, constructor, constructordecl, destructordecl, destructor,
        argument, index, block, type, init, op, literal, modifier, memberlist, classn, structn,
        super_list, super, preproc, whilestmt, forstmt, ifstmt, nonterminal, macro, classblock, functionblock,
        specifier, typedefexpr, empty, tokenstring, MAXENUMVALUE = empty};
    class srcSAXEventContext{
        public:
            srcSAXEventContext(){
                triggerField = std::vector<unsigned short int>(MAXENUMVALUE, 0);
            }
            std::vector<int> genericDepth;
            unsigned int currentLineNumber;
            std::vector<unsigned short int> triggerField;
            std::string currentFilePath, currentFileName, currentFileLanguage, currentsrcMLRevision, currentToken;
            bool sawgeneric;
            std::size_t depth;
            
            inline bool And(std::vector<ParserState> vec) const{
                for(auto field : vec){
                    if(triggerField[field]) continue;
                    else return false;
                }
                return true;
            }
            inline bool Nand(std::vector<ParserState> vec) const{
                for(auto field : vec){
                    if(triggerField[field]) return false;
                    else continue;
                }
                return true;
            }
            inline bool Or(std::vector<ParserState> vec) const{
                for(auto field : vec){
                    if(triggerField[field]) return true;
                    else continue;
                }
                return false;
            }
            inline bool Nor(std::vector<ParserState> vec) const{
                for(auto field : vec){
                    if(triggerField[field]) return false;
                    else continue;
                }
                return true;
            }
            inline bool IsEqualTo(ParserState lhs, ParserState rhs) const{
                return triggerField[lhs] == triggerField[rhs] ? true : false;
            }
            inline bool IsGreaterThan(ParserState lhs, ParserState rhs) const{
                return triggerField[lhs] > triggerField[rhs] ? true : false;
            }
            inline bool IsGreaterThanOrEqualTo(ParserState lhs, ParserState rhs) const{
                return triggerField[lhs] >= triggerField[rhs] ? true : false;   
            }
            inline bool IsLessThan(ParserState lhs, ParserState rhs) const{
                return triggerField[lhs] < triggerField[rhs] ? true : false;    
            }
            inline bool IsLessThanOrEqualTo(ParserState lhs, ParserState rhs) const{
                return triggerField[lhs] <= triggerField[rhs] ? true : false;   
            }
            inline bool IsOpen(ParserState field) const{
                if(triggerField[field]) return true;
                else return false;
            }
            inline bool IsClosed(ParserState field) const{
                if(triggerField[field]) return false;
                else return true;
            }
    };
    class EventListener {

        protected:
           std::unordered_map<srcSAXEventDispatch::ParserState, std::function<void(const srcSAXEventDispatch::srcSAXEventContext&)>, std::hash<int>> openEventMap, closeEventMap;


        public:

            EventListener() {
                DefaultEventHandlers();
            }

            virtual void HandleEvent() {}
            virtual void HandleEvent(srcSAXEventDispatch::ParserState pstate, srcSAXEventDispatch::ElementState estate, const srcSAXEventDispatch::srcSAXEventContext& ctx) {

                switch(estate){

                    case srcSAXEventDispatch::ElementState::open: {
                        auto event = openEventMap.find(pstate);
                        if(event != openEventMap.end()){
                            event->second(ctx);
                        }
                        break;
                    }

                    case srcSAXEventDispatch::ElementState::close: {
                        auto event = closeEventMap.find(pstate);
                        if(event != closeEventMap.end()){
                            event->second(ctx);
                        }
                        break;
                    }

                    default:
                        throw std::runtime_error("Something went terribly, terribly wrong");

                }

            }

        protected:
            void NopOpenEvents(std::initializer_list<ParserState> states) {

                for(ParserState state : states) {

                    openEventMap[state] = [this](const srcSAXEventContext& ctx) {};

                }

            }            
            void NopCloseEvents(std::initializer_list<ParserState> states) {

                for(ParserState state : states) {

                    closeEventMap[state] = [this](const srcSAXEventContext& ctx) {};

                }

            } 

        private:

            void DefaultEventHandlers() {
                using namespace srcSAXEventDispatch;

                NopOpenEvents({
                    ParserState::declstmt,
                    ParserState::exprstmt,
                    ParserState::parameterlist,
                    ParserState::ifstmt,
                    ParserState::forstmt,
                    ParserState::whilestmt,
                    ParserState::templates,
                    ParserState::argumentlist,
                    ParserState::call,
                    ParserState::function,
                    ParserState::constructor,
                    ParserState::functiondecl,
                    ParserState::destructordecl,
                    ParserState::constructordecl,
                    ParserState::classn,
                    ParserState::structn,
                    ParserState::destructor,
                    ParserState::parameter,
                    ParserState::memberlist,
                    ParserState::index,
                    ParserState::op,
                    ParserState::block,
                    ParserState::init,
                    ParserState::argument,
                    ParserState::literal,
                    ParserState::modifier,
                    ParserState::decl,
                    ParserState::type,
                    ParserState::typedefexpr,
                    ParserState::expr,
                    ParserState::name,
                    ParserState::macro,
                    ParserState::specifier,
                });

                NopCloseEvents({
                    ParserState::declstmt,
                    ParserState::exprstmt,
                    ParserState::parameterlist,
                    ParserState::ifstmt,
                    ParserState::forstmt,
                    ParserState::whilestmt,
                    ParserState::templates,
                    ParserState::argumentlist,
                    ParserState::call,
                    ParserState::function,
                    ParserState::constructor,
                    ParserState::destructor,
                    ParserState::functiondecl,
                    ParserState::constructordecl,
                    ParserState::destructordecl,
                    ParserState::classn,
                    ParserState::structn,
                    ParserState::parameter,
                    ParserState::memberlist,
                    ParserState::index,
                    ParserState::op,
                    ParserState::block,
                    ParserState::init,
                    ParserState::argument,
                    ParserState::literal,
                    ParserState::modifier,
                    ParserState::decl,
                    ParserState::type,
                    ParserState::typedefexpr,
                    ParserState::expr,
                    ParserState::name,
                    ParserState::macro,
                    ParserState::tokenstring,
                    ParserState::specifier,
                });

        }

    };
    class PolicyDispatcher;
    class PolicyListener{

        public:

            PolicyListener() {}
            virtual void Notify(const PolicyDispatcher * policy) = 0;

        };
    class EventDispatcher{
    public:
        virtual void AddListener(EventListener* l) = 0;
        virtual void RemoveListener(EventListener* l) = 0;
    protected:
        std::vector<EventListener*> elementListeners;
        virtual void DispatchEvent(ParserState, ElementState, const srcSAXEventContext&) = 0;
    };

    class PolicyDispatcher{
    public:
        PolicyDispatcher(std::initializer_list<PolicyListener *> listeners) : policyListeners(listeners){}
        virtual void AddListener(PolicyListener* listener){
            policyListeners.push_back(listener);
        }
        virtual void RemoveListener(PolicyListener* listener){
            policyListeners.erase(std::remove(policyListeners.begin(), policyListeners.end(), listener), policyListeners.end());
        }

        template<typename T>
        T * Data() const {

            return static_cast<T *>(DataInner());

        }

    protected:
        std::vector<PolicyListener*> policyListeners;
        virtual void * DataInner() const = 0;
        virtual void NotifyAll() {

            for(PolicyListener * const listener : policyListeners)
                listener->Notify(this);

        }

    };

}

#endif
