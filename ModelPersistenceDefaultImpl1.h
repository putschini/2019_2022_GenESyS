/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ModelPersistenceDefaultImpl1.h
 * Author: rafael.luiz.cancian
 *
 * Created on 20 de Maio de 2019, 20:41
 */

#ifndef MODELPERSISTENCEDEFAULTIMPL1_H
#define MODELPERSISTENCEDEFAULTIMPL1_H

#include "ModelPersistence_if.h"
#include "Model.h"

class ModelPersistenceDefaultImpl1 : public ModelPersistence_if{
public:
    ModelPersistenceDefaultImpl1(Model* model);
    ModelPersistenceDefaultImpl1(const ModelPersistenceDefaultImpl1& orig);
    virtual ~ModelPersistenceDefaultImpl1();
public:
    virtual bool save(std::string filename);
    virtual bool load(std::string filename);
    virtual bool isSaved();
private:
    //bool saveAsTXT(std::string filename);
    //bool loadAsTXT(std::string filename);
    void _saveContent(std::list<std::string>* content, std::ofstream* file);
    std::list<std::string>* _adjustFieldsToSaveAsTXT(std::list<std::string>* fields);
//    std::list<std::string>* _adjustFieldsToSaveAsXML(std::list<std::string>* fields);
    std::list<std::string>* _getModelInfosFieldsToSave();
    std::list<std::string>* _getSimulatorInfoFieldsToSave();
private:
    Model* _model = nullptr;
    bool _isSaved = false;
    std::string _linefieldseparator = "; ";
};

#endif /* MODELPERSISTENCEDEFAULTIMPL1_H */

