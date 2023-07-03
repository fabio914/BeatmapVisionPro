import SwiftUI
import BeatmapLoader

struct BeatmapSelection: Identifiable, Codable, Hashable {
    let id = UUID()
    let url: URL
    let difficulty: BeatmapDifficulty
}

struct MenuScreen: View {
    @Environment(\.openImmersiveSpace) var openImmersiveSpace
    @Environment(\.dismissImmersiveSpace) var dismissImmersiveSpace

    @State var selection: BeatmapSelection?
    @StateObject var beatmapFileManager = BeatmapFileManager()

    var body: some View {
        NavigationView {
            VStack(alignment: /*@START_MENU_TOKEN@*/.center/*@END_MENU_TOKEN@*/) {
                NavigationLink(destination: DownloadScreen(viewModel: .init(beatmapFileManager: beatmapFileManager))) {
                    Text("Download Beatmaps")
                }
                ForEach(beatmapFileManager.filePreviews) { filePreview in
                    NavigationLink(destination: SongScreen(selection: $selection, filePreview: filePreview)) {
                        HStack {
                            Image(uiImage: filePreview.preview.coverImage)
                                .resizable()
                                .aspectRatio(contentMode: .fit)
                                .frame(width: 40, height: 40)
                            Text(filePreview.preview.songName)
                        }
                    }
                }
            }
            .navigationTitle("Songs")
        }
        .onAppear(perform: beatmapFileManager.reloadPreviews)
        .onChange(of: selection) { _, newValue in
            Task {
                if let selection = newValue {
                    await openImmersiveSpace(value: selection)
                }
//                else {
//                    await dismissImmersiveSpace()
//                }
            }
        }
    }
}
