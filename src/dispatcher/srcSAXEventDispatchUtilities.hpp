#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <initializer_list>


#ifndef INCLUDED_SRCSAX_EVENT_DISPATCH_UTILITIES_HPP
#define INCLUDED_SRCSAX_EVENT_DISPATCH_UTILITIES_HPP

namespace srcSAXEventDispatch{
    template <typename ...policies>
    class srcSAXEventDispatcher;
    class srcSAXEventContext;            
    enum ElementState {open, close};
    enum ParserState {decl, expr, parameter, declstmt, exprstmt, parameterlist, 
        argumentlist, argumentlisttemplate, call, templates, ctrlflow, endflow, genericargumentlist,
        name, function, functiondecl, constructor, constructordecl, destructordecl, destructor,
        argument, index, block, type, init, op, literal, modifier, memberlist, classn, structn,
        super_list, super, publicaccess, privateaccess, protectedaccess, preproc, whilestmt, forstmt, 
        ifstmt, nonterminal, macro, classblock, functionblock, specifier, typedefexpr, empty, tokenstring,
        ifblock, whileblock, forblock, MAXENUMVALUE = empty};
    class EventListener {
        private:
            bool dispatched;
        protected:
           std::unordered_map<srcSAXEventDispatch::ParserState, std::function<void(srcSAXEventDispatch::srcSAXEventContext&)>, std::hash<int>> openEventMap, closeEventMap;


        public:
            friend srcSAXEventContext;

            EventListener() : dispatched(false) {
                DefaultEventHandlers();
            }

            virtual void HandleEvent() { dispatched = true; }
            virtual void HandleEvent(srcSAXEventDispatch::ParserState pstate, srcSAXEventDispatch::ElementState estate, srcSAXEventDispatch::srcSAXEventContext& ctx) {

                if(dispatched) return;

                dispatched = true;

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
                    ParserState::publicaccess,
                    ParserState::protectedaccess,
                    ParserState::privateaccess,
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
                    ParserState::publicaccess,
                    ParserState::protectedaccess,                  
                    ParserState::privateaccess,
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
            virtual void Notify(const PolicyDispatcher * policy, const srcSAXEventContext & ctx) = 0;

        };
    class EventDispatcher{
    public:
        virtual void AddListener(EventListener* l) = 0;
        virtual void RemoveListener(EventListener* l) = 0;
    protected:
        std::list<EventListener*> elementListeners;
        virtual void DispatchEvent(ParserState, ElementState) = 0;
    };

    class PolicyDispatcher{
    public:
        PolicyDispatcher(std::initializer_list<PolicyListener *> listeners) : policyListeners(listeners){}
        virtual void AddListener(PolicyListener* listener){
            policyListeners.push_back(listener);
        }
        virtual void RemoveListener(PolicyListener* listener){
            policyListeners.erase(std::find(policyListeners.begin(), policyListeners.end(), listener));
        }

        template<typename T>
        T * Data() const {

            return static_cast<T *>(DataInner());

        }

    protected:
        std::list<PolicyListener*> policyListeners;
        virtual void * DataInner() const = 0;
        virtual void NotifyAll(const srcSAXEventContext & ctx) {
            for(std::list<PolicyListener*>::iterator listener = policyListeners.begin(); listener != policyListeners.end(); ++listener){
                (*listener)->Notify(this, ctx);
            }

        }

    };
    class srcSAXEventContext : public EventDispatcher {
        public:
            srcSAXEventContext() = delete;
            srcSAXEventContext(const std::vector<std::string> & elementStack) : elementStack(elementStack) {
                triggerField = std::vector<unsigned short int>(MAXENUMVALUE, 0);
                depth = 0;
                dispatching = false;
            }
            std::list<EventListener*> elementListeners;
            const std::vector<std::string> & elementStack;
            std::vector<int> genericDepth;
            unsigned int currentLineNumber;
            std::vector<unsigned short int> triggerField;
            std::string currentFilePath, currentFileName, currentFileLanguage, currentsrcMLRevision, currentToken;
            bool sawgeneric;
            std::size_t depth;

        private:
            bool dispatching;
            ParserState currentPState;
            ElementState currentEState;
        public:

            void AddListener(EventListener* listener){
                elementListeners.push_back(listener);
            }
            void AddListenerDispatch(EventListener* listener){
                if(dispatching)
                    listener->HandleEvent(currentPState, currentEState, *this);
                AddListener(listener);
            }
            void AddListenerNoDispatch(EventListener* listener){
                if(dispatching)
                    listener->dispatched = true;
                AddListener(listener);
            }
            void RemoveListener(EventListener* listener){
                elementListeners.erase(std::find(elementListeners.begin(), elementListeners.end(), listener));
            }
            void RemoveListenerDispatch(EventListener* listener){
                if(dispatching)
                    listener->HandleEvent(currentPState, currentEState, *this);
                RemoveListener(listener);
            }
            void RemoveListenerNoDispatch(EventListener* listener){
                if(dispatching)
                    listener->dispatched = true;
                RemoveListener(listener);
            }
            void DispatchEvent(ParserState pstate, ElementState estate){

                dispatching = true;
                currentPState = pstate;
                currentEState = estate;

                for(std::list<EventListener*>::iterator listener = elementListeners.begin(); listener != elementListeners.end(); ++listener ){
                    (*listener)->HandleEvent(pstate, estate, *this);
                }
                for(std::list<EventListener*>::iterator listener = elementListeners.begin(); listener != elementListeners.end(); ++listener ){
                    (*listener)->dispatched = false;
                }

                dispatching = false;

            }
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

}

#endif