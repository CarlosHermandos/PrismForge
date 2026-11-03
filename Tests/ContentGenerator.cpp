#include "Plugin.h"
#include <fstream>
#include <iostream>
#include <map>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    juce::ScopedJuceInitialiser_GUI init;
    auto directory = juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
    if (directory.createDirectory().failed())
        return 1;
    std::ofstream parameters(
        directory.getChildFile("Parameters.tsv").getFullPathName().toStdString());
    parameters << "id\tname\tunit\tminimum\tmaximum\tdefault\tskew\tstep\n";
    for (auto p : prism::parameters)
        parameters << p.id << '\t' << p.name << '\t' << p.unit << '\t' << p.min << '\t' << p.max
                   << '\t' << p.def << '\t' << p.skew << '\t' << p.step << '\n';
    auto folder = directory.getChildFile("Factory Presets");
    folder.createDirectory();
    std::ofstream presets(directory.getChildFile("Presets.tsv").getFullPathName().toStdString());
    presets << "index\tname\tcategory\ttags\tauthor\tdescription\tmacro1\tmacro2\tmacro3\tmacro4\n";
    auto processor = std::make_unique<PrismProcessor>();
    std::map<std::string, int> counts;
    for (size_t i = 0; i < prism::presets.size(); ++i)
    {
        auto &p = prism::presets[i];
        processor->setCurrentProgram(int(i));
        juce::MemoryBlock state;
        processor->getStateInformation(state);
        auto file = folder.getChildFile(juce::File::createLegalFileName(p.name) + ".prism");
        if (!file.replaceWithData(state.getData(), state.getSize()))
            return 2;
        presets << i << '\t' << p.name << '\t' << p.category << '\t' << p.tags << '\t' << p.author
                << '\t' << p.description;
        for (auto name : p.macroNames)
            presets << '\t' << name;
        presets << '\n';
        ++counts[p.category];
    }
    for (auto &[category, count] : counts)
        std::cout << category << ": " << count << '\n';
    std::cout << "Exported " << prism::presets.size() << " real program files and "
              << prism::paramCount << " parameter definitions.\n";
}
