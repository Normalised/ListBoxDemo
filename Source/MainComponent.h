#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// The data model. Sends change messages when the list of files gets modified
struct Playlist : public juce::ChangeBroadcaster
{
    void setFiles(std::vector<juce::File> newFiles)
    {
        files = newFiles;
        sendChangeMessage();
    }

    void addFile(juce::File file)
    {
        files.push_back(file);
        sendChangeMessage();
    }

    // Add other methods here, addFiles, clear, removeFile etc
    // just remember to sendChangeMessage when the vector of files gets modified

    size_t size() const
    {
        return files.size();
    }

    std::vector<juce::File> files;
};

// Wrap the actual Playlist inside the juce ListBoxModel so the ListBox can display it.
struct PlaylistListboxModel : public juce::ListBoxModel
{
    PlaylistListboxModel(Playlist& playlistToUse) : playlist(playlistToUse) {}

    // This is really the only method in ListBoxModel that I would call a 'model' method
    int getNumRows() override
    {
        return static_cast<int>(playlist.size());
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if(rowNumber < getNumRows())
        {
            // Paint the item here.
            const auto item = playlist.files.at(rowNumber);
            g.setColour(juce::Colours::white);
            g.drawSingleLineText(item.getFileNameWithoutExtension(), 4, height);
        }
    }

    // Keep a reference to the playlist, not a copy
    Playlist& playlist;
};

// Put the listbox inside its own component
struct PlaylistView : public juce::Component, juce::ChangeListener
{
    PlaylistView(Playlist& playlistToUse) : playlist(playlistToUse)
    {
        addAndMakeVisible(listBox);
        listBox.setBounds(0, 0, 400, 400);
        // Listen to the playlist model for changes.
        playlist.addChangeListener(this);        
    }

    // Call updateContent on the listbox when the playlist model changes
    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        listBox.updateContent();
    }

    // Keep a reference to the playlist, not a copy.
    Playlist& playlist;

    // Wrap the playlist model with the ListBoxModel adapter
    PlaylistListboxModel listboxModel{ playlist };

    juce::ListBox listBox {"Playlist", &listboxModel};

};

// Put the file chooser inside its own component
struct ChooserView : public juce::Component
{
    ChooserView(Playlist& playlistToUse) : playlist(playlistToUse)
    {
        addAndMakeVisible(chooseFileButton);
        chooseFileButton.setButtonText("Choose");
        chooseFileButton.setBounds(10, 10, 80, 30);
        chooseFileButton.onClick = [this]()
        {
            selectFile();
        };


        fileChooser = std::make_unique<juce::FileChooser> ("Please select the file you want to load...",
                                                           juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                                                         "*.wav");        
    }

    void selectFile() const
    {
        constexpr auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        // Capture 'this' in the lambda so that the 'playlist' is available
        fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& chooser)
        {
            const juce::File wavFile(chooser.getResult());

            playlist.addFile(wavFile);
        });        
    }

    // Keep a reference to the playlist, not a copy.
    Playlist& playlist;

    juce::TextButton chooseFileButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

};

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        setSize(800, 600);

        addAndMakeVisible(chooserView);
        addAndMakeVisible(playlistView);

        chooserView.setBounds(0, 0, 100, 40);
        playlistView.setBounds(100, 10, 300, 400);
    }

private:

    // Create the data model
    Playlist playlist;
    // And pass it into the components that use it.
    PlaylistView playlistView { playlist };
    ChooserView chooserView { playlist };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
