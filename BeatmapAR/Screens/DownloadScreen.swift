import Foundation
import SwiftUI

enum DownloadScreenError: Error {
    case invalidURL
    case downloadFailed
    case failedToSave
}

enum DownloadScreenViewState {
    case ready
    case downloading(URL)
    case failed(DownloadScreenError)
}

final class DownloadScreenViewModel: ObservableObject {

    @Published private(set) var viewState: DownloadScreenViewState = .ready
    @Published var beatmapId: String = ""
    private weak var beatmapFileManager: BeatmapFileManager?

    var canStartDownload: Bool {
        guard case .ready = viewState,
            !beatmapId.isEmpty,
            CharacterSet(charactersIn: beatmapId).isSubset(of: .alphanumerics)
        else {
            return false
        }
        return true
    }

    init(beatmapFileManager: BeatmapFileManager) {
        self.beatmapFileManager = beatmapFileManager
    }

    func startDownload() {
        guard canStartDownload else { return }

        guard let url = URL(string: "https://api.beatsaver.com/download/key/\(beatmapId)") else {
            viewState = .failed(.invalidURL)
            return
        }

        viewState = .downloading(url)
    }

    @MainActor
    func downloadTask() async {
        guard case .downloading(let url) = viewState else {
            return
        }

        let urlRequest = URLRequest(url: url)

        guard let (data, _) = try? await URLSession.shared.data(for: urlRequest) else {
            viewState = .failed(.downloadFailed)
            return
        }

        do {
            try beatmapFileManager?.save(data, beatmapId: beatmapId)
            viewState = .ready
        } catch {
            viewState = .failed(.failedToSave)
        }
    }

    func reset() {
        if case .failed = viewState {
            viewState = .ready
        }
    }
}

struct DownloadScreen: View {

    @StateObject var viewModel: DownloadScreenViewModel

    enum FocusedField {
        case beatmapField
    }

    @FocusState private var focusedField: FocusedField?

    @ViewBuilder
    var content: some View {
        switch viewModel.viewState {
        case .ready:
            VStack {
                Text("Enter the Beatmap ID from bsaber.com for the map you want to download.")
                    .font(.body)
                Text("For example, you should type 90f4 if the URL for the map is bsaber.com/songs/90f4/")
                    .font(.callout)
                Text("Beatmaps with arcs and new note types are not supported yet.")
                    .font(.callout)
                    .padding(.bottom, 50)
                TextField(text: $viewModel.beatmapId, prompt: Text("Beatmap ID")) {
                    Text("Beatmap ID")
                }
                .focused($focusedField, equals: .beatmapField)
                Button("Download", action: viewModel.startDownload)
                    .disabled(!viewModel.canStartDownload)
            }
            .padding()
            .onAppear {
                focusedField = .beatmapField
            }
        case .downloading:
            VStack {
                Text("Downloading...")
                ProgressView()
                    .progressViewStyle(CircularProgressViewStyle())
            }
            .padding()
            .task {
                await viewModel.downloadTask()
            }
        case .failed(let downloadScreenError):
            VStack {
                Text("Unable to download map!")
                    .font(.body)
                switch downloadScreenError {
                case .invalidURL:
                    Text("Invalid Beatmap URL")
                case .downloadFailed:
                    Text("Download failed")
                case .failedToSave:
                    Text("Failed to save map. This Beatmap might not be compatible with the app.")
                }
                Button("OK", action: viewModel.reset)
            }
            .padding()
        }
    }

    var body: some View {
        content
            .navigationTitle("Download Beatmap")
    }
}
