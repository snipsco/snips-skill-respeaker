import ConfigParser
import io

CONFIGURATION_ENCODING_FORMAT = "utf-8"

class SnipsConfigParser(ConfigParser.SafeConfigParser):
    def to_dict(self):
        return {section: {option_name : option for option_name, option in self.items(section)} for section in self.sections()}

    @staticmethod
    def read_configuration_file(configuration_file):
        try:
            with io.open(configuration_file, encoding=CONFIGURATION_ENCODING_FORMAT) as f:
                conf_parser = SnipsConfigParser()
                conf_parser.readfp(f)
                return conf_parser.to_dict()
        except (IOError, ConfigParser.Error) as e:
            print(e)
            return dict()
