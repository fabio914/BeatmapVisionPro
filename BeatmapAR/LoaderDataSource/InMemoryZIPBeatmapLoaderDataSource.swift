import Foundation
import ZIPFoundation
import BeatmapLoader

final class InMemoryZIPBeatmapLoaderDataSource {

    let data: Data
    private lazy var archive: Archive? = Archive(data: data, accessMode: .read)

    init(with data: Data) {
        self.data = data
    }
}

extension InMemoryZIPBeatmapLoaderDataSource: BeatmapLoaderDataSourceProtocol {

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
