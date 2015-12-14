#include "undostack.h"

UndoStack* UndoStack::ptr2UndoStack = nullptr;

UndoStack::UndoStack(){

}

UndoStack* UndoStack::getInstance(){
    if(UndoStack::ptr2UndoStack == nullptr){
        UndoStack::ptr2UndoStack = new UndoStack();
    }
    return UndoStack::ptr2UndoStack;
}

void UndoStack::push(QImage historyImage){
    this->historyStack.push(historyImage);
}

QImage UndoStack::pop(){
    if(this->historyStack.empty()){
        return this->initImage;
    }else{
        QImage result;
        result = this->historyStack.top();
        this->historyStack.pop();
        return result;
    }
}

void UndoStack::init(QImage image){
    std::stack<QImage> newStack;
    this->historyStack.swap(newStack);
    this->initImage = image;
}
