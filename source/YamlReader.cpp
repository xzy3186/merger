#include "YamlReader.hpp"

const std::string YamlReader::kMsgPrefix("[YamlReader]:");

YamlReader::YamlReader(const std::string &yaml_node_name)
{
    try {
       yaml_ = YamlParameter::GetInstance()[yaml_node_name];
       if( !yaml_ ) {
         throw kMsgPrefix + "No node named \"" + yaml_node_name + "\" found in the config file.";
       }
    }
    catch (YAML::Exception& e) {
      std::cout << kMsgPrefix << "exception " << e.what() << std::endl;
      throw;
    }
    catch (std::string msg){
      std::cout << msg << std::endl;
      throw;
    }
}

std::string YamlReader::GetString(const std::string &attr, const bool &throw_error, const std::string &default_str) const
{
    try {
       if( !yaml_[attr] ) {
           if(throw_error) {
               throw kMsgPrefix + "No node named \"" + attr + "\" found in the config file.";
           } else {
               std::cout << kMsgPrefix << "Loaded default value " << attr << " = " << default_str << std::endl;
               return default_str;
           }
       }
       std::cout << kMsgPrefix << "Loaded " << attr << " = " << yaml_[attr].as<std::string>() << std::endl;
    }
    catch (YAML::Exception& e) {
      std::cout << kMsgPrefix << "exception " << e.what() << std::endl;
      throw;
    }
    catch (std::string msg){
      std::cout << msg << std::endl;
      throw;
    }
    return yaml_[attr].as<std::string>();
}

unsigned long long YamlReader::GetULong64(const std::string &attr, const bool &throw_error, const unsigned long long &default_val) const
{
    try {
       if( !yaml_[attr] ) {
           if(throw_error) {
               throw kMsgPrefix + "No node named \"" + attr + "\" found in the config file.";
           } else {
               std::cout << kMsgPrefix << "Loaded default value " << attr << " = " << default_val << std::endl;
               return default_val;
           }
       }
       std::cout << kMsgPrefix << "Loaded " << attr << " = " << yaml_[attr].as<unsigned long long>() << std::endl;
    }
    catch (YAML::Exception& e) {
      std::cout << kMsgPrefix << "exception " << e.what() << std::endl;
      throw;
    }
    catch (std::string msg){
      std::cout << msg << std::endl;
      throw;
    }
    return yaml_[attr].as<unsigned long long>();
}

double YamlReader::GetDouble(const std::string &attr, const bool &throw_error, const double &default_val) const
{
    try {
       if( !yaml_[attr] ) {
           if(throw_error) {
               throw kMsgPrefix + "No node named \"" + attr + "\" found in the config file.";
           } else {
               std::cout << kMsgPrefix << "Loaded default value " << attr << " = " << default_val << std::endl;
               return default_val;
           }
       }
       std::cout << kMsgPrefix << "Loaded " << attr << " = " << yaml_[attr].as<double>() << std::endl;
    }
    catch (YAML::Exception& e) {
      std::cout << kMsgPrefix << "exception " << e.what() << std::endl;
      throw;
    }
    catch (std::string msg){
      std::cout << msg << std::endl;
      throw;
    }
    return yaml_[attr].as<double>();
}
