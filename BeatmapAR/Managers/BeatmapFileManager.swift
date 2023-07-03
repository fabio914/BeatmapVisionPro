import Foundation
import BeatmapLoader

struct BeatmapFilePreview: Identifiable {
    let id = UUID()
    let preview: BeatmapPreview
    let url: URL
}

final class BeatmapFileManager: ObservableObject {

    @Published private(set) var filePreviews: [BeatmapFilePreview] = []

    let directoryURL: URL
    private let zipExtension = "zip"

    init(directoryURL: URL) {
        self.directoryURL = directoryURL
    }

    init!() {
        let manager = FileManager.default
        guard let documentsURL = manager.urls(for: .documentDirectory, in: .userDomainMask).first else {
            return nil
        }

        self.directoryURL = documentsURL
    }

    private func loadFilePreviews() -> [BeatmapFilePreview] {
        // TODO: Move this to a background thread and load this asynchronously.

        let manager = FileManager.default
        guard let contents = try? manager.contentsOfDirectory(at: directoryURL, includingPropertiesForKeys: nil) else {
            return []
        }

        return contents
            .filter({ $0.pathExtension == zipExtension })
            .compactMap({ url in
                let zipDataSource = ZIPBeatmapLoaderDataSource(with: url)
                let loader = BeatmapLoader(dataSource: zipDataSource)
                guard let preview = try? loader.loadPreview() else { return nil }
                return .init(preview: preview, url: url)
            })
    }

    func reloadPreviews() {
        filePreviews = loadFilePreviews()
    }

    enum BeatmapFileManagerError: Error {
        case invalidFile
    }

    func save(_ data: Data, beatmapId: String) throws {
        let zipDataSource = InMemoryZIPBeatmapLoaderDataSource(with: data)
        let loader = BeatmapLoader(dataSource: zipDataSource)
        guard let _ = try? loader.loadPreview() else {
            throw BeatmapFileManagerError.invalidFile
        }

        let fileURL = directoryURL.appending(path: "\(beatmapId).zip", directoryHint: .notDirectory)
        try data.write(to: fileURL)

        reloadPreviews()
    }
}
