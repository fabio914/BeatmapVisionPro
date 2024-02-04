//
//  OggDecoder.m
//  
//
//  Created by Arkadiusz Pituła on 10/04/2022.
//

#import <Foundation/Foundation.h>
#import "oggHelper.h"
#import "OGGDecoder.h"

@implementation OGGDecoder
dispatch_queue_t _decodeQueue = dispatch_queue_create("com.oggDecoder.decodingQueue", DISPATCH_QUEUE_CONCURRENT);

- (BOOL)decode:(NSURL *)oggFile into:(NSURL *)outputFile {
    oggHelper helper;
    const char *fileInChar = [[oggFile path] cStringUsingEncoding:NSASCIIStringEncoding];
    const char *fileOutChar = [[outputFile path] cStringUsingEncoding:NSASCIIStringEncoding];

    int output = helper.decode(fileInChar, fileOutChar);
    return output == 1;
}

- (NSURL *)decode:(NSURL *)oggFile {
    NSURL* output = [[[NSFileManager defaultManager] temporaryDirectory] URLByAppendingPathComponent:@"output.wav"];
    if ([self decode:oggFile into:output] == TRUE) {
        return output;
    }
    return nil;
}

- (void)decode:(NSURL *)oggFile completion:(void (^)(NSURL * _Nullable))completion {
    dispatch_async(_decodeQueue, ^{
        NSURL* output = [self decode:oggFile];
        dispatch_async(dispatch_get_main_queue(), ^(void){
            completion(output);
        });
    });
}

- (void)decode:(NSURL *)oggFile into:(NSURL *)outputFile completion:(void (^)(BOOL))completion {
    dispatch_async(_decodeQueue, ^{
        BOOL result = [self decode:oggFile into:outputFile];
        dispatch_async(dispatch_get_main_queue(), ^(void){
            completion(result);
        });
    });
}

@end
