#include <assert.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>


struct kvm {
    int sys_fd;
    int vm_fd;
    int api_ver;
    char *mem;
};

struct vcpu {
    int fd;
    int mmap_size;
    struct kvm_run *kvm_run;
};

void kvm_init(struct kvm *kvm) {
    struct kvm_userspace_memory_region mem_reg;
    int ret;

    kvm->sys_fd = open("/dev/kvm", O_RDWR);
    assert(kvm->sys_fd >= 0);

    kvm->api_ver = ioctl(kvm->sys_fd, KVM_GET_API_VERSION, 0);
    assert(kvm->api_ver == 12);

    kvm->vm_fd = ioctl(kvm->sys_fd, KVM_CREATE_VM, (unsigned long)0);
    assert(kvm->vm_fd >= 0);

    kvm->mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    assert(kvm->mem != MAP_FAILED);

    mem_reg.slot = 0;
    mem_reg.flags = 0;
    mem_reg.guest_phys_addr = 0x1000;
    mem_reg.memory_size = 0x1000;
    mem_reg.userspace_addr = (unsigned long)kvm->mem;
    ret = ioctl(kvm->vm_fd, KVM_SET_USER_MEMORY_REGION, &mem_reg);
    assert(ret >= 0);
}

void kvm_del(struct kvm *kvm) {
    int ret;

    ret = munmap(kvm->mem, 0x1000);
    assert(ret >= 0);

    ret = close(kvm->vm_fd);
    assert(ret >= 0);

    ret = close(kvm->sys_fd);
    assert(ret >= 0);
}

void vcpu_init(struct kvm *kvm, struct vcpu *vcpu) {

    vcpu->fd = ioctl(kvm->vm_fd, KVM_CREATE_VCPU, 0);
    assert(vcpu->fd >= 0);

    vcpu->mmap_size = ioctl(kvm->sys_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    assert(vcpu->mmap_size > 0);

    vcpu->kvm_run = mmap(NULL, vcpu->mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu->fd, 0);
    assert(vcpu->kvm_run != MAP_FAILED);
}

void vcpu_del(struct vcpu *vcpu) {
    int ret;

    ret = munmap(vcpu->kvm_run, vcpu->mmap_size);
    assert(ret >= 0);

    ret = close(vcpu->fd);
    assert(ret >= 0);
}

int main(void) {
    struct kvm kvm;
    struct vcpu vcpu;

    // Init
    kvm_init(&kvm);

    vcpu_init(&kvm, &vcpu);

    // Cleanup
    vcpu_del(&vcpu);

    kvm_del(&kvm);

    return 0;
}
