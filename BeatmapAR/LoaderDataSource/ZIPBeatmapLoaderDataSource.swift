import Foundation
import ZIPFoundation
import BeatmapLoader

final class ZIPBeatmapLoaderDataSource {

    let zipFileURL: URL
    private lazy var archive: Archive? = Archive(url: zipFileURL, accessMode: .read)

    init(with zipFileURL: URL) {
        self.zipFileURL = zipFileURL
    }
}

extension ZIPBeatmapLoaderDataSource: BeatmapLoaderDataSourceProtocol {

    func loader(_ loader: BeatmapLoader, dataForFileNamed fileName: String) -> Data? {
        var result = Data()

        guard let entry = archive?[fileName],
            let _ = try? archive?.extract(entry, consumer: { chunk in result.append(chunk) })
        else {
            return nil
        }

        return result
    }
}
